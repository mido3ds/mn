#include "mn/OS.h"
#include "mn/Virtual_Memory.h"
#include "mn/Thread.h"
#include "mn/Debug.h"
#include "mn/File.h"
#include "mn/Memory.h"
#include "mn/Str.h"

#include <stdio.h>
#include <stdlib.h>

namespace mn
{
	Allocator
	virtual_allocator()
	{
		static Allocator _allocator = allocator_custom_new(nullptr,
		[](void*, size_t size, uint8_t) { return virtual_alloc(nullptr, size); },
		[](void*, Block block) { virtual_free(block); });
		return _allocator;
	}


	struct Memory_Block
	{
		size_t size = 0;
		Str callstack;
		Memory_Block *next = nullptr, *prev = nullptr;
	};

	struct Leak_Detector
	{
		Memory_Block* head;
		Mutex mtx;

		Leak_Detector()
		{
			head = nullptr;
			mtx = mutex_new();
		}

		~Leak_Detector()
		{
			//do stuff here
			if(head)
			{
				size_t count = 0;
				size_t size = 0;
				auto it = head;
				while(it)
				{
					::fprintf(stderr, "Leak size: %zu, call stack:\n", it->size);
					if(it->callstack.count == 0)
						::fprintf(stderr, "run in debug mode to get call stack info\n");
					else
						::fprintf(stderr, "%s\n", it->callstack.ptr);

					++count;
					size += it->size;
					str_free(it->callstack);
					it = it->next;
				}
				::fprintf(stderr, "Leaks count: %zu, Leaks size(bytes): %zu\n", count, size);
			}
		}
	};
	static Leak_Detector _leak_detector;

	Allocator
	leak_detector()
	{
		static Allocator _allocator = allocator_custom_new(&_leak_detector,
		[](void* user_data, size_t size, uint8_t){
			Leak_Detector* self = (Leak_Detector*)user_data;
			Memory_Block* ptr = (Memory_Block*)::malloc(size + sizeof(Memory_Block));

			if(ptr == nullptr)
				return Block{};

			ptr->size = size;
			ptr->next = self->head;
			ptr->prev = nullptr;
			mutex_lock(self->mtx);
				if(self->head) self->head->prev = ptr;
				self->head = ptr;
			mutex_unlock(self->mtx);

			ptr->callstack = callstack_dump(clib_allocator);
			return Block { ptr + 1, size };
		},
		[](void* user_data, Block block){
			Leak_Detector* self = (Leak_Detector*)user_data;
			Memory_Block* ptr = ((Memory_Block*)block.ptr) - 1;

			mutex_lock(self->mtx);
				if(ptr == self->head)
					self->head = ptr->next;

				if(ptr->prev)
					ptr->prev->next = ptr->next;

				if(ptr->next)
					ptr->next->prev = ptr->prev;
			mutex_unlock(self->mtx);

			str_free(ptr->callstack);
			::free(ptr);
		});
		return _allocator;
	}

	void
	_panic(const char* cause)
	{
		::fprintf(stderr, "[PANIC]: %s\n%s\n", cause, callstack_dump().ptr);
		::exit(-1);
	}

	Str
	file_content_str(const char* filename, Allocator allocator)
	{
		Str str = str_with_allocator(allocator);
		File f = file_open(filename, IO_MODE::READ, OPEN_MODE::OPEN_ONLY);
		if(file_valid(f) == false)
			panic("cannot read file \"{}\"", filename);
		
		buf_resize(str, file_size(f) + 1);
		--str.count;
		str.ptr[str.count] = '\0';

		size_t read_size = file_read(f, Block { str.ptr, str.count });
		((void)(read_size));
		assert(read_size == str.count);

		file_close(f);
		return str;
	}
}