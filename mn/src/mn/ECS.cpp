#include "mn/ECS.h"

namespace mn
{
	Entity
	_entity_new()
	{
		static std::atomic<uint32_t> id = 0;
		return Entity{id.fetch_add(1) + 1};
	}
}