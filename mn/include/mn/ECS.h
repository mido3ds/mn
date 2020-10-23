#pragma once

#include "mn/Exports.h"
#include "mn/Thread.h"
#include "mn/Pool.h"
#include "mn/Memory.h"
#include "mn/Buf.h"
#include "mn/Map.h"
#include "mn/OS.h"

#include <stdint.h>
#include <atomic>
#include <assert.h>

namespace mn
{
	// entity
	struct Entity
	{
		uint32_t id;
		operator uint32_t() const { return id; }
		bool operator==(const Entity& other) const { return id == other.id; }
		bool operator!=(const Entity& other) const { return id != other.id; }
	};
	inline Entity null_entity{};

	inline static Entity
	clone(const Entity& self)
	{
		return self;
	}

	template<>
	struct Hash<Entity>
	{
		inline size_t
		operator()(const Entity& entity) const
		{
			return size_t(entity.id);
		}
	};

	MN_EXPORT Entity
	entity_new();

	// backing pool storage
	template<typename T>
	struct Ref_Component
	{
		std::atomic<int32_t> atomic_rc;
		Entity entity;
		T component;
	};

	template<typename T>
	struct Store
	{
		std::atomic<int32_t> atomic_rc;
		Mutex mtx;
		Pool pool;
	};

	template<typename T>
	inline static Store<T>*
	store_new()
	{
		auto self = alloc<Store<T>>();
		self->atomic_rc = 1;
		self->mtx = mutex_new("Store mutex");
		self->pool = pool_new(sizeof(Ref_Component<T>), 64);
		return self;
	}

	template<typename T>
	inline static void
	store_free(Store<T>* self)
	{
		store_unref(self);
	}

	template<typename T>
	inline static void
	destruct(Store<T>* self)
	{
		store_free(self);
	}

	template<typename T>
	inline static Store<T>*
	store_ref(Store<T>* self)
	{
		self->atomic_rc.fetch_add(1);
		return self;
	}

	template<typename T>
	inline static void
	store_unref(Store<T>* self)
	{
		auto rc = self->atomic_rc.fetch_sub(1);
		assert(rc >= 1);
		if (rc == 1)
		{
			pool_free(self->pool);
			mutex_free(self->mtx);
			free(self);
		}
	}

	template<typename T>
	inline static Ref_Component<T>*
	store_component_new(Store<T>* self)
	{
		mutex_lock(self->mtx);
		auto ptr = (Ref_Component<T>*)pool_get(self->pool);
		mutex_unlock(self->mtx);

		ptr->atomic_rc = 1;
		return ptr;
	}

	template<typename T>
	inline static void
	store_component_free(Store<T>* self, Ref_Component<T>* ptr)
	{
		store_component_unref(self, ptr);
	}

	template<typename T>
	inline static Ref_Component<T>*
	store_component_ref(Ref_Component<T>* ptr)
	{
		ptr->atomic_rc.fetch_add(1);
		return ptr;
	}

	template<typename T>
	inline static void
	store_component_unref(Store<T>* self, Ref_Component<T>* ptr)
	{
		auto rc = ptr->atomic_rc.fetch_sub(1);
		assert(rc >= 1);
		if (rc == 1)
		{
			destruct(ptr->component);

			mutex_lock(self->mtx);
			pool_put(self->pool, ptr);
			mutex_unlock(self->mtx);
		}
	}

	// reference bag
	template<typename T>
	struct Ref_Bag
	{
		Store<T>* store;
		Buf<Ref_Component<T>*> components;
		Map<Entity, size_t> table;
		uint32_t version;
	};

	template<typename T>
	inline static Ref_Bag<T>
	ref_bag_new()
	{
		Ref_Bag<T> self{};
		self.store = store_new<T>();
		self.components = buf_new<Ref_Component<T>*>();
		self.table = map_new<Entity, size_t>();
		self.version = 0;
		return self;
	}

	template<typename T>
	inline static void
	ref_bag_free(Ref_Bag<T>& self)
	{
		for (auto c: self.components)
			store_component_unref(self.store, c);

		store_free(self.store);
		buf_free(self.components);
		map_free(self.table);
	}

	template<typename T>
	inline static void
	destruct(Ref_Bag<T>& self)
	{
		ref_bag_free(self);
	}

	template<typename T>
	inline static Ref_Bag<T>
	ref_bag_clone(const Ref_Bag<T>& self)
	{
		Ref_Bag<T> other{};
		other.store = store_ref(self.store);
		other.components = buf_memcpy_clone(self.components);
		other.table = map_memcpy_clone(self.table);
		other.version = self.version;

		for (auto c: other.components)
			store_component_ref(c);

		return other;
	}

	template<typename T>
	inline static Ref_Bag<T>
	clone(const Ref_Bag<T>& self)
	{
		return ref_bag_clone(self);
	}

	template<typename T>
	inline static const T*
	ref_bag_read(const Ref_Bag<T>& self, Entity e)
	{
		if (auto it = map_lookup(self.table, e))
			return &self.components[it->value]->component;
		return nullptr;
	}

	template<typename T>
	inline static bool
	ref_bag_has(const Ref_Bag<T>& self, Entity e)
	{
		return ref_bag_read(self, e) != nullptr;
	}

	template<typename T>
	inline static T*
	ref_bag_write(Ref_Bag<T>& self, Entity e)
	{
		T* res = nullptr;
		if (auto it = map_lookup(self.table, e))
		{
			auto ptr = self.components[it->value];
			if (ptr->atomic_rc.load() == 1)
			{
				res = &ptr->component;
			}
			else
			{
				auto new_ptr = store_component_new(self.store);
				new_ptr->component = clone(ptr->component);
				new_ptr->entity = e;
				store_component_unref(self.store, ptr);
				self.components[it->value] = new_ptr;
				res = &new_ptr->component;
			}
		}
		else
		{
			auto new_ptr = store_component_new(self.store);
			::memset(&new_ptr->component, 0, sizeof(new_ptr->component));
			new_ptr->entity = e;

			auto ix = self.components.count;
			buf_push(self.components, new_ptr);
			map_insert(self.table, e, ix);
			res = &new_ptr->component;
		}
		self.version++;
		return res;
	}

	template<typename T>
	inline static void
	ref_bag_remove(Ref_Bag<T>& self, Entity e)
	{
		auto it = map_lookup(self.table, e);
		if (it == nullptr)
			return;

		auto remove_ix = it->value;
		store_component_unref(self.store, self.components[remove_ix]);
		buf_remove(self.components, remove_ix);
		map_remove(self.table, e);

		if (remove_ix < self.components.count)
		{
			auto replace_it = map_lookup(self.table, self.components[remove_ix]->entity);
			replace_it->value = remove_ix;
		}
		self.version++;
	}

	// value bag
	template<typename T>
	struct Val_Component
	{
		Entity entity;
		T component;
	};

	template<typename T>
	inline static Val_Component<T>
	clone(const Val_Component<T>& self)
	{
		Val_Component<T> res{};
		res.entity = self.entity;
		res.component = clone(self.component);
		return res;
	}

	template<typename T>
	struct Val_Bag
	{
		Buf<Val_Component<T>> components;
		Map<Entity, size_t> table;
		uint32_t version;
	};

	template<typename T>
	inline static Val_Bag<T>
	val_bag_new()
	{
		Val_Bag<T> self{};
		self.components = buf_new<Val_Component<T>>();
		self.table = map_new<Entity, size_t>();
		self.version = 0;
		return self;
	}

	template<typename T>
	inline static void
	val_bag_free(Val_Bag<T>& self)
	{
		for (auto& c: self.components)
			destruct(c.component);
		buf_free(self.components);
		map_free(self.table);
	}

	template<typename T>
	inline static void
	destruct(Val_Bag<T>& self)
	{
		val_bag_free(self);
	}

	template<typename T>
	inline static Val_Bag<T>
	val_bag_clone(const Val_Bag<T>& self)
	{
		Val_Bag<T> other{};
		other.components = buf_clone(self.components);
		other.table = map_memcpy_clone(self.table);
		other.version = self.version;
		return other;
	}

	template<typename T>
	inline static Val_Bag<T>
	clone(const Val_Bag<T>& self)
	{
		return val_bag_clone(self);
	}

	template<typename T>
	inline static T
	val_bag_get(const Val_Bag<T>& self, Entity e)
	{
		if (auto it = map_lookup(self.table, e))
			return self.components[it->value].component;
		assert(false && "unreachable");
		return T{};
	}

	template<typename T>
	inline static void
	val_bag_set(Val_Bag<T>& self, Entity e, const T& v)
	{
		if (auto it = map_lookup(self.table, e))
		{
			destruct(self.components[it->value].component);
			self.components[it->value].component = v;
		}
		else
		{
			Val_Component<T> new_component{};
			new_component.entity = e;
			new_component.component = v;
			auto ix = self.components.count;
			buf_push(self.components, new_component);
			map_insert(self.table, e, ix);
		}
		++self.version;
	}

	template<typename T>
	inline static void
	val_bag_remove(Val_Bag<T>& self, Entity e)
	{
		auto it = map_lookup(self.table, e);
		if (it == nullptr)
			return;

		auto remove_ix = it->value;
		destruct(self.components[remove_ix]);
		buf_remove(self.components, remove_ix);
		map_remove(self.table, e);

		if (remove_ix < self.components.count)
		{
			auto replace_it = map_lookup(self.table, self.components[remove_ix]->entity);
			replace_it->value = remove_ix;
		}
		self.version++;
	}
}