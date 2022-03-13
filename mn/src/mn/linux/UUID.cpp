#include "mn/UUID.h"
#include "mn/Assert.h"

#include <sys/random.h>

namespace mn
{
	UUID
	uuid_generate()
	{
		UUID self{};

		// use /dev/urandom to generate random uuid
		// /dev/urandom is faster and as-secure-as /dev/random, see https://www.2uo.de/myths-about-urandom
		// $ man getrandom
		// 	"Using getrandom() to read small buffers (<= 256 bytes) from the urandom source is the preferred mode of usage."
		mn::Block buffer {&self, sizeof(self)};
		ssize_t size = getrandom(buffer.ptr, buffer.size, 0);
		mn_assert((size_t)size == buffer.size);
		(void)size;

		// version 4
		self.bytes[6] = (self.bytes[6] & 0x0f) | 0x40;
		// variant is 10
		self.bytes[8] = (self.bytes[8] & 0x3f) | 0x80;
		return self;
	}
} // namespace mn