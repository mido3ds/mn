#include "mn/ECS.h"

namespace mn
{
	Entity
	entity_new()
	{
		std::atomic<uint32_t> id = 0;
		return Entity{id.fetch_add(1) + 1};
	}
}