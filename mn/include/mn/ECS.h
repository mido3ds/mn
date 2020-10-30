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
#include <type_traits>

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
	_entity_new();

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

	struct Tag_Bag
	{
		Set<Entity> table;
	};

	inline static Tag_Bag
	tag_bag_new()
	{
		return Tag_Bag{};
	}

	inline static void
	tag_bag_free(Tag_Bag& self)
	{
		set_free(self.table);
	}

	inline static void
	destruct(Tag_Bag& self)
	{
		tag_bag_free(self);
	}

	inline static Tag_Bag
	tag_bag_clone(const Tag_Bag& self)
	{
		Tag_Bag other{};
		other.table = set_memcpy_clone(self.table);
		return other;
	}

	inline static Tag_Bag
	clone(const Tag_Bag& self)
	{
		return tag_bag_clone(self);
	}

	inline static bool
	tag_bag_has(const Tag_Bag& self, Entity e)
	{
		return set_lookup(self.table, e) != nullptr;
	}

	inline static void
	tag_bag_add(Tag_Bag& self, Entity e)
	{
		set_insert(self.table, e);
	}

	inline static void
	tag_bag_remove(Tag_Bag& self, Entity e)
	{
		set_remove(self.table, e);
	}


	template<typename T>
	inline static size_t
	typehash()
	{
		static const size_t _hash = typeid(T).hash_code();
		return _hash;
	}

	struct Abstract_World_Table
	{
		virtual ~Abstract_World_Table() = default;
		virtual const char* _name() const = 0;
		virtual const void* _read(Entity e) const = 0;
		virtual void* _write(Entity e) = 0;
		virtual void _remove(Entity e) = 0;
		virtual const Map<Entity, size_t>* _entities() const = 0;
	};

	template<typename T>
	struct World_Table final: Abstract_World_Table
	{
		Str name;
		Ref_Bag<T> bag;

		World_Table(Str _name)
			: name(_name)
		{
			bag = ref_bag_new<T>();
		}

		~World_Table() override
		{
			str_free(name);
			ref_bag_free(bag);
		}

		const char*
		_name() const override
		{
			return name.ptr;
		}

		const void*
		_read(Entity e) const override
		{
			return ref_bag_read(bag, e);
		}

		void*
		_write(Entity e) override
		{
			return ref_bag_write(bag, e);
		}

		void
		_remove(Entity e) override
		{
			return ref_bag_remove(bag, e);
		}

		const Map<Entity, size_t>*
		_entities() const override
		{
			return &bag.table;
		}
	};

	struct World
	{
		Set<Entity> alive_entities;
		Map<size_t, Abstract_World_Table*> tables;
		Map<size_t, Tag_Bag> tags;

		Entity
		entity_new()
		{
			auto e = _entity_new();
			set_insert(alive_entities, e);
			return e;
		}

		void
		entity_free(Entity e)
		{
			assert(set_lookup(alive_entities, e) && "invalid entity");
			set_remove(alive_entities, e);
			for (auto [_, table]: tables)
				table->_remove(e);
			for (auto& [_, bag]: tags)
				tag_bag_remove(bag, e);
		}

		template<typename T>
		const T*
		read(Entity e) const
		{
			static_assert(std::is_empty_v<T> == false, "empty structs should be tags");
			assert(set_lookup(alive_entities, e) && "invalid entity");
			return (const T*)_read(typehash<T>(), e);
		}

		const void*
		_read(size_t type_hash, Entity e) const
		{
			auto table = map_lookup(tables, type_hash);
			return table->value->_read(e);
		}

		template<typename T>
		T*
		write(Entity e)
		{
			static_assert(std::is_empty_v<T> == false, "empty structs should be tags");
			assert(set_lookup(alive_entities, e) && "invalid entity");
			return (T*)_write(typehash<T>(), e);
		}

		void*
		_write(size_t type_hash, Entity e)
		{
			auto table = map_lookup(tables, type_hash);
			return table->value->_write(e);
		}

		template<typename T>
		void
		remove(Entity e)
		{
			static_assert(std::is_empty_v<T> == false, "empty structs should be tags");
			assert(set_lookup(alive_entities, e) && "invalid entity");
			_remove(typehash<T>(), e);
		}

		void
		_remove(size_t type_hash, Entity e)
		{
			auto table = map_lookup(tables, type_hash);
			return table->value->_remove(e);
		}

		template<typename T>
		Buf<Entity>
		list() const
		{
			auto res = buf_with_allocator<Entity>(memory::tmp());
			if constexpr (std::is_empty_v<T>)
			{
				auto t = typehash<T>();
				auto it = map_lookup(tags, t);
				buf_reserve(res, it->value.table.count);
				for (auto e: it->value.table)
					buf_push(res, e);
			}
			else
			{
				auto t = typehash<T>();
				auto it = map_lookup(tables, t);
				auto entities = it->value->_entities();
				buf_reserve(res, entities->count);
				for (auto [e, _]: *entities)
					buf_push(res, e);
			}
			return res;
		}

		template<typename T>
		bool
		tag_has(Entity e) const
		{
			static_assert(std::is_empty_v<T>);
			assert(set_lookup(alive_entities, e) && "invalid entity");

			auto t = typehash<T>();
			auto b = map_lookup(tags, t);
			return tag_bag_has(b->value, e);
		}

		template<typename T>
		void
		tag_add(Entity e)
		{
			static_assert(std::is_empty_v<T>);
			assert(set_lookup(alive_entities, e) && "invalid entity");

			auto t = typehash<T>();
			auto b = map_lookup(tags, t);
			tag_bag_add(b->value, e);
		}

		template<typename T>
		void
		tag_remove(Entity e)
		{
			static_assert(std::is_empty_v<T>);
			assert(set_lookup(alive_entities, e) && "invalid entity");

			auto t = typehash<T>();
			auto b = map_lookup(tags, t);
			tag_bag_remove(b->value, e);
		}
	};

	struct World_Schema
	{
		Map<size_t, Abstract_World_Table*> tables;
		Map<size_t, Tag_Bag> tags;
	};

	inline static World_Schema*
	world_schema_new()
	{
		return alloc_zerod<World_Schema>();
	}

	inline static void
	world_schema_free(World_Schema* self)
	{
		for(auto [_, table]: self->tables)
			free_destruct(table);
		map_free(self->tables);
		for (auto [_, b]: self->tags)
			tag_bag_free(b);
		map_free(self->tags);
		free(self);
	}

	inline static void
	destruct(World_Schema* self)
	{
		world_schema_free(self);
	}

	template<typename T>
	inline static void
	world_schema_create_table(World_Schema* self, const char* name)
	{
		auto t = typehash<T>();
		if (map_lookup(self->tables, t))
			panic("world_create_table('{}') failed because type already exists", name);

		if constexpr (std::is_empty_v<T> == false)
		{
			auto table = alloc_construct<World_Table<T>>(str_from_c(name));
			map_insert(self->tables, t, (Abstract_World_Table*)table);
		}
		else
		{
			map_insert(self->tags, t, tag_bag_new());
		}
	}
	#define mn_world_schema_create_table(w, ...) mn::world_schema_create_table<__VA_ARGS__>(w, #__VA_ARGS__)



	inline static World*
	world_new(World_Schema* schema)
	{
		auto self = alloc_zerod<World>();
		self->tables = schema->tables;
		schema->tables = map_new<size_t, Abstract_World_Table*>();

		self->tags = schema->tags;
		schema->tags = map_new<size_t, Tag_Bag>();

		return self;
	}

	inline static void
	world_free(World* world)
	{
		set_free(world->alive_entities);
		for(auto [_, table]: world->tables)
			free_destruct(table);
		map_free(world->tables);
		for (auto [_, b]: world->tags)
			tag_bag_free(b);
		map_free(world->tags);
		free(world);
	}

	inline static void
	destruct(World* self)
	{
		world_free(self);
	}
}