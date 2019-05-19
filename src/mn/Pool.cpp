#include "mn/Pool.h"
#include "mn/Memory.h"

namespace mn
{
	struct IPool
	{
		Allocator meta_allocator;
		Allocator arena;
		void* head;
		size_t element_size;
	};

	Pool
	pool_new(size_t element_size, size_t bucket_size, Allocator meta_allocator)
	{
		Pool self = alloc_from<IPool>(meta_allocator);

		if(element_size < sizeof(void*))
			element_size = sizeof(void*);

		self->meta_allocator = meta_allocator;
		self->arena = allocator_arena_new(element_size * bucket_size, meta_allocator);
		self->head = nullptr;
		self->element_size = element_size;
		return self;
	}

	void
	pool_free(Pool self)
	{
		allocator_free(self->arena);
		free_from(self->meta_allocator, self);
	}

	void*
	pool_get(Pool self)
	{
		if(self->head != nullptr)
		{
			void* result = self->head;
			self->head = (void*)(*(size_t*)self->head);
			return result;
		}

		return alloc_from(self->arena, self->element_size, alignof(char)).ptr;
	}

	void
	pool_put(Pool self, void* ptr)
	{
		size_t* sptr = (size_t*)ptr;
		*sptr = (size_t)self->head;
		self->head = ptr;
	}
}