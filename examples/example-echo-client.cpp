#include <mn/IO.h>
#include <mn/Socket.h>
#include <mn/Defer.h>
#include <mn/Assert.h>

int
main()
{
	auto socket = mn::socket_open(mn::SOCKET_FAMILY_IPV4, mn::SOCKET_TYPE_TCP);
	mn_assert_msg(socket, "socket_open failed");
	mn_defer{mn::socket_close(socket);};

	bool status = mn::socket_connect(socket, "localhost", "4000");
	mn_assert_msg(status, "socket_connect failed");
	mn_defer{mn::socket_disconnect(socket);};

	auto line = mn::str_new();
	mn_defer{mn::str_free(line);};
	size_t read_bytes = 0, write_bytes = 0;
	do
	{
		mn::readln(line);
		if(line == "quit")
			break;
		else if(line.count == 0)
			continue;

		mn::print("you write: '{}'\n", line);

		write_bytes = mn::socket_write(socket, mn::block_from(line));
		mn_assert_msg(write_bytes == line.count, "socket_write failed");

		mn::str_resize(line, 1024);
		auto [read_bytes_count, err] = socket_read(socket, mn::block_from(line), mn::INFINITE_TIMEOUT);
		if (err)
		{
			mn::print("socket_read error");
			break;
		}
		read_bytes = read_bytes_count;
		mn_assert(read_bytes == write_bytes);

		mn::str_resize(line, read_bytes);
		mn::print("server: '{}'\n", line);
	} while(read_bytes > 0);

	return 0;
}