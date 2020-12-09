#pragma once

#include "mn/Stream.h"
#include "mn/Str.h"
#include "mn/Result.h"

namespace mn
{
	enum SOCKET_FAMILY
	{
		SOCKET_FAMILY_UNSPEC,
		SOCKET_FAMILY_IPV4,
		SOCKET_FAMILY_IPV6,
	};

	enum SOCKET_TYPE
	{
		SOCKET_TYPE_TCP,
		SOCKET_TYPE_UDP
	};

	enum MN_SOCKET_ERROR
	{
		// no error
		MN_SOCKET_ERROR_OK,
		// something went wrong, but we don't check for it specifically for now
		// note: this is sort of catch all category for all unhandled errors, any kind
		// of errors except the ones listed below
		MN_SOCKET_ERROR_GENERIC_ERROR,
		// failed to allocate memory buffer
		MN_SOCKET_ERROR_OUT_OF_MEMORY,
		// internal mn error
		MN_SOCKET_ERROR_INTERNAL_ERROR,
		// function failed due to timeout
		MN_SOCKET_ERROR_TIMEOUT,
		// function faild becuase of failure to send/recv from socket
		MN_SOCKET_ERROR_CONNECTION_CLOSED,
	};

	inline static const char*
	mn_socket_error_message(MN_SOCKET_ERROR e)
	{
		switch(e)
		{
		case MN_SOCKET_ERROR_OK:
			return "no error";
		case MN_SOCKET_ERROR_GENERIC_ERROR:
			return "something went wrong, this is a general error message";
		case MN_SOCKET_ERROR_OUT_OF_MEMORY:
			return "failed to allocate memory buffer";
		case MN_SOCKET_ERROR_INTERNAL_ERROR:
			return "internal mn error";
		case MN_SOCKET_ERROR_TIMEOUT:
			return "error due to timeout";
		default:
			assert(false && "unreachable");
			return "something went wrong, this shouldn't happen!!!!!!";
		}
	}

	typedef struct ISocket* Socket;
	struct ISocket final: IStream
	{
		int64_t handle;
		SOCKET_FAMILY family;
		SOCKET_TYPE type;

		MN_EXPORT virtual void
		dispose() override;

		MN_EXPORT virtual size_t
		read(Block data) override;

		MN_EXPORT virtual size_t
		write(Block data) override;

		MN_EXPORT virtual int64_t
		size() override;

		virtual int64_t
		cursor_operation(STREAM_CURSOR_OP, int64_t) override
		{
			assert(false && "sockets doesn't support cursor operations");
			return STREAM_CURSOR_ERROR;
		}
	};

	MN_EXPORT Socket
	socket_open(SOCKET_FAMILY family, SOCKET_TYPE type);

	MN_EXPORT void
	socket_close(Socket self);

	inline static void
	destruct(Socket self)
	{
		socket_close(self);
	}

	MN_EXPORT bool
	socket_connect(Socket self, const Str& address, const Str& port);

	inline static bool
	socket_connect(Socket self, const char* address, const char* port)
	{
		return socket_connect(self, str_lit(address), str_lit(port));
	}

	MN_EXPORT bool
	socket_bind(Socket self, const Str& port);

	inline static bool
	socket_bind(Socket self, const char* port)
	{
		return socket_bind(self, str_lit(port));
	}

	MN_EXPORT bool
	socket_listen(Socket self, int max_connections = 0);

	MN_EXPORT Socket
	socket_accept(Socket self, Timeout timeout);

	MN_EXPORT void
	socket_disconnect(Socket self);

	MN_EXPORT Result<size_t, MN_SOCKET_ERROR>
	socket_read(Socket self, Block data, Timeout timeout);

	MN_EXPORT size_t
	socket_write(Socket self, Block data);

	MN_EXPORT int64_t
	socket_fd(Socket self);
}
