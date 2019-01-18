#include <catch2/catch.hpp>

#include <mn/Memory.h>
#include <mn/Buf.h>
#include <mn/Str.h>
#include <mn/Map.h>
#include <mn/Pool.h>
#include <mn/Memory_Stream.h>
#include <mn/Virtual_Memory.h>
#include <mn/IO.h>
#include <mn/Str_Intern.h>
#include <mn/Ring.h>
#include <mn/OS.h>

using namespace mn;

TEST_CASE("memory allocation", "[mn]")
{
	SECTION("Case 01")
	{
		Block b = alloc(sizeof(int), alignof(int));
		CHECK(b.ptr != nullptr);
		CHECK(b.size != 0);

		free(b);
	}

	SECTION("Case 02")
	{
		Allocator stack = allocator_stack_new(1024);
		CHECK(allocator_top() == clib_allocator);

		allocator_push(stack);
		CHECK(allocator_top() == stack);

		Block b = alloc(512, alignof(char));
		free(b);

		allocator_pop();
		CHECK(allocator_top() == clib_allocator);

		allocator_free(stack);
	}

	SECTION("Case 03")
	{
		Allocator arena = allocator_arena_new(512);
		CHECK(allocator_top() == clib_allocator);

		allocator_push(arena);
		CHECK(allocator_top() == arena);

		for(int i = 0; i < 1000; ++i)
			int* ptr = alloc<int>();

		allocator_pop();
		CHECK(allocator_top() == clib_allocator);

		allocator_free(arena);
	}

	SECTION("Case 04")
	{
		{
			Str name = str_with_allocator(allocator_tmp());
			str_pushf(name, "Name: %s", "Mostafa");
			CHECK(name == str_lit("Name: Mostafa"));
		}

		allocator_tmp_free();

		{
			Str name = str_with_allocator(allocator_tmp());
			str_pushf(name, "Name: %s", "Mostafa");
			CHECK(name == str_lit("Name: Mostafa"));
		}

		allocator_tmp_free();
	}
}

TEST_CASE("buf", "[mn]")
{
	SECTION("Case 01")
	{
		auto arr = buf_new<int>();
		for(int i = 0; i < 10; ++i)
			buf_push(arr, i);
		for(size_t i = 0; i < arr.count; ++i)
			CHECK(i == arr[i]);
		buf_free(arr);
	}

	SECTION("Case 02")
	{
		auto arr = buf_new<int>();
		for(int i = 0; i < 10; ++i)
			buf_push(arr, i);
		size_t i = 0;
		for(auto num: arr)
			CHECK(num == i++);
		buf_free(arr);
	}

	SECTION("Case 03")
	{
		auto arr = buf_new<int>();
		for(int i = 0; i < 10; ++i)
			buf_push(arr, i);
		CHECK(buf_empty(arr) == false);
		for(size_t i = 0; i < 10; ++i)
			buf_pop(arr);
		CHECK(buf_empty(arr) == true);
		buf_free(arr);
	}
}

TEST_CASE("str", "[mn]")
{
	SECTION("Case 01")
	{
		Str str = str_new();

		str_push(str, "Mostafa");
		CHECK("Mostafa" == str);

		str_push(str, " Saad");
		CHECK(str == str_lit("Mostafa Saad"));

		str_push(str, " Abdel-Hameed");
		CHECK(str == str_lit("Mostafa Saad Abdel-Hameed"));

		str_pushf(str, " age: %d", 25);
		CHECK(str == "Mostafa Saad Abdel-Hameed age: 25");

		str_free(str);
	}

	SECTION("Case 02")
	{
		Str str = str_new();
		str_null_terminate(str);
		CHECK(str == "");
		CHECK(str.count == 0);

		buf_pushn(str, 5, 'a');
		str_null_terminate(str);
		CHECK(str == "aaaaa");
		str_free(str);
	}
}

TEST_CASE("map", "[mn]")
{
	SECTION("Case 01")
	{
		auto num = map_new<int, int>();

		for(int i = 0; i < 10; ++i)
			map_insert(num, i, i + 10);

		for(int i = 0; i < 10; ++i)
		{
			CHECK(map_lookup(num, i)->key == i);
			CHECK(map_lookup(num, i)->value == i + 10);
		}

		for(int i = 10; i < 20; ++i)
			CHECK(map_lookup(num, i) == nullptr);

		for(int i = 0; i < 10; ++i)
		{
			if(i % 2 == 0)
				map_remove(num, i);
		}

		for(int i = 0; i < 10; ++i)
		{
			if(i % 2 == 0)
				CHECK(map_lookup(num, i) == nullptr);
			else
			{
				CHECK(map_lookup(num, i)->key == i);
				CHECK(map_lookup(num, i)->value == i + 10);
			}
		}

		int i = 0;
		for(auto it = map_begin(num);
			it != map_end(num);
			it = map_next(num, it))
		{
			++i;
		}
		CHECK(i == 5);

		// for(auto it = mn::map_begin(num); it != mn::map_end(num); it = mn::map_next(num, it))
		// {
		// 	::printf("(%d, %d)\n", it->key, it->value);
		// }

		map_free(num);
	}
}

TEST_CASE("Pool", "[mn]")
{
	SECTION("Case 01")
	{
		Pool pool = pool_new(sizeof(int), 1024);
		int* ptr = (int*)pool_get(pool);
		CHECK(ptr != nullptr);
		*ptr = 234;
		pool_put(pool, ptr);
		int* new_ptr = (int*)pool_get(pool);
		CHECK(new_ptr == ptr);
		pool_free(pool);
	}
}

TEST_CASE("Memory_Stream", "[mn]")
{
	SECTION("Case 01")
	{
		Memory_Stream mem = memory_stream_new();
		CHECK(memory_stream_size(mem) == 0);
		CHECK(memory_stream_cursor_pos(mem) == 0);
		memory_stream_write(mem, block_lit("Mostafa"));
		CHECK(memory_stream_size(mem) == 7);
		CHECK(memory_stream_cursor_pos(mem) == 7);

		char name[8] = {0};
		CHECK(memory_stream_read(mem, block_from(name)) == 0);
		CHECK(memory_stream_cursor_pos(mem) == 7);

		memory_stream_cursor_to_start(mem);
		CHECK(memory_stream_cursor_pos(mem) == 0);

		CHECK(memory_stream_read(mem, block_from(name)) == 7);
		CHECK(memory_stream_cursor_pos(mem) == 7);

		CHECK(::strcmp(name, "Mostafa") == 0);
		memory_stream_free(mem);
	}
}

TEST_CASE("virtual memory", "[mn]")
{
	SECTION("Case 01")
	{
		size_t size = 1ULL * 1024ULL * 1024ULL * 1024ULL;
		Block block = virtual_alloc(nullptr, size);
		CHECK(block.ptr != nullptr);
		CHECK(block.size == size);
		virtual_free(block);
	}
}

TEST_CASE("reads", "[mn]")
{
	SECTION("Case 01")
	{
		int a, b;
		float c, d;
		Str e = str_new();
		size_t read_count = reads("-123 20 1.23 0.123 Mostafa ", a, b, c, d, e);
		CHECK(read_count == 5);
		CHECK(a == -123);
		CHECK(b == 20);
		CHECK(c == 1.23f);
		CHECK(d == 0.123f);
		CHECK(e == "Mostafa");
		str_free(e);
	}

	SECTION("Case 02")
	{
		Reader reader = reader_wrap_str(nullptr, str_lit("Mostafa Saad"));
		Str str = str_new();
		size_t read_count = readln(reader, str);
		CHECK(read_count == 12);
		CHECK(str == "Mostafa Saad");

		str_free(str);
		reader_free(reader);
	}
}

TEST_CASE("path os encoding", "[mn]")
{
	SECTION("Case 01")
	{
		auto os_path = path_os_encoding("C:/bin/my_file.exe");

		#if OS_WINDOWS
			CHECK(os_path == "C:\\bin\\my_file.exe");
		#endif

		str_free(os_path);
	}
}

TEST_CASE("path exists", "[mn]")
{
	/*CHECK(path_exists("mn/mn.lua"));
	CHECK(path_is_file("mn/mn.lua"));
	CHECK(path_is_folder("mn"));
	Str ss = str_new();
	printfmt("sanitize: {}\n", path_join(ss, "Mostafa", "Saad", "Abdelhameed"));
	printfmt("pwd: {}\n", path_absolute("mn/mn.lua"));
	printfmt("pwd: {}\n", path_current());
	printfmt("copied: {}\n", file_copy(path_absolute("README.md"), path_absolute("README2.md")));
	printfmt("folder copied: {}\n", folder_copy(path_absolute("cpprelude/"), path_absolute("cpprelude2/")));
	printfmt("folder moved: {}\n", folder_move(path_absolute("cpprelude2"), path_absolute("cpprelude3")));
	printfmt("folder removed: {}\n", folder_remove(path_absolute("cpprelude3")));
	printfmt("moved: {}\n", file_move(path_absolute("README2.md"), path_absolute("mn/README2.md")));
	printfmt("deleted: {}\n", file_remove(path_absolute("mn/README2.md")));
	auto entries = path_entries("cpprelude");
	for (const auto& entry : entries)
		if (entry.kind == Path_Entry::KIND_FILE)
			printfmt("File: {}\n", entry.name);
		else
			printfmt("Folder: {}\n", entry.name);
	destruct(entries);
	path_current_change("/home");
	printfmt("pwd: {}\n", path_current());
	CHECK(path_current() == "/home");*/
}

TEST_CASE("Str_Intern", "[mn]")
{
	SECTION("Case 01")
	{
		Str_Intern intern = str_intern_new();

		const char* is = str_intern(intern, "Mostafa");
		CHECK(is != nullptr);
		CHECK(is == str_intern(intern, "Mostafa"));

		const char* big_str = "my name is Mostafa";
		const char* begin = big_str + 11;
		const char* end = begin + 7;
		CHECK(is == str_intern(intern, begin, end));

		str_intern_free(intern);
	}
}

TEST_CASE("Ring", "[mn]")
{
	SECTION("Case 01")
	{
		allocator_push(leak_detector());

		Ring<int> r = ring_new<int>();

		for(int i = 0; i < 10; ++i)
			ring_push_back(r, i);

		for(size_t i = 0; i < r.count; ++i)
			CHECK(r[i] == i);

		for(int i = 0; i < 10; ++i)
			ring_push_front(r, i);

		for(int i = 9; i >= 0; --i)
		{
			CHECK(ring_back(r) == i);
			ring_pop_back(r);
		}

		for(int i = 9; i >= 0; --i)
		{
			CHECK(ring_front(r) == i);
			ring_pop_front(r);
		}

		ring_free(r);

		allocator_pop();
	}

	SECTION("Case 02")
	{
		allocator_push(leak_detector());
		Ring<Str> r = ring_new<Str>();

		for(int i = 0; i < 10; ++i)
			ring_push_back(r, str_from_c("Mostafa"));

		for(int i = 0; i < 10; ++i)
			ring_push_front(r, str_from_c("Saad"));

		for(int i = 4; i >= 0; --i)
		{
			CHECK(ring_back(r) == "Mostafa");
			str_free(ring_back(r));
			ring_pop_back(r);
		}

		for(int i = 4; i >= 0; --i)
		{
			CHECK(ring_front(r) == "Saad");
			str_free(ring_front(r));
			ring_pop_front(r);
		}

		destruct(r);

		allocator_pop();
	}
}
