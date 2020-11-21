#include "mn/memory/Arena.h"
#include "mn/IO.h"
#include <assert.h>

namespace mn::memory
{
	Arena::Arena(size_t block_size, Interface* meta)
	{
		assert(block_size != 0);
		this->meta = meta;
		this->root = nullptr;
		this->block_size = block_size;
		this->total_mem = 0;
		this->used_mem = 0;
		this->highwater_mem = 0;
		this->clear_all_readjust_threshold = 4ULL * 1024ULL * 1024ULL;
		this->clear_all_current_highwater = 0;
		this->clear_all_previous_highwater = 0;
	}

	Arena::~Arena()
	{
		free_all();
	}

	Block
	Arena::alloc(size_t size, uint8_t)
	{
		grow(size);

		uint8_t* ptr = this->root->alloc_head;
		this->root->alloc_head += size;
		this->used_mem += size;
		this->highwater_mem = this->highwater_mem > this->used_mem ? this->highwater_mem : this->used_mem;
		this->clear_all_current_highwater = this->clear_all_current_highwater > this->used_mem ? this->clear_all_current_highwater : this->used_mem;

		return Block{ ptr, size };
	}

	void
	Arena::free(Block block)
	{
		this->used_mem -= block.size;
	}

	void
	Arena::grow(size_t size)
	{
		if (this->root != nullptr)
		{
			size_t node_used_mem = this->root->alloc_head - (uint8_t*)this->root->mem.ptr;
			size_t node_free_mem = this->root->mem.size - node_used_mem;
			if (node_free_mem >= size)
				return;
		}

		size_t request_size = size > this->block_size ? size : this->block_size;
		request_size += sizeof(Node);

		Node* new_node = (Node*)meta->alloc(request_size, alignof(int)).ptr;
		this->total_mem += request_size - sizeof(Node);

		new_node->mem.ptr = &new_node[1];
		new_node->mem.size = request_size - sizeof(Node);
		new_node->alloc_head = (uint8_t*)new_node->mem.ptr;
		new_node->next = this->root;
		this->root = new_node;
	}

	void
	Arena::free_all()
	{
		while (this->root)
		{
			Node* next = this->root->next;
			meta->free(Block{ this->root, this->root->mem.size + sizeof(Node) });
			this->root = next;
		}
		this->root = nullptr;
		this->total_mem = 0;
		this->used_mem = 0;
	}

	void
	Arena::clear_all()
	{
		size_t delta = 0;
		if (this->clear_all_current_highwater > this->clear_all_previous_highwater)
			delta = this->clear_all_current_highwater - this->clear_all_previous_highwater;
		else
			delta = this->clear_all_previous_highwater - this->clear_all_current_highwater;

		bool readjust = delta >= this->clear_all_readjust_threshold;

		if ((this->root && this->root->next != nullptr) || readjust)
		{
			this->free_all();
			this->grow(this->clear_all_current_highwater);
			this->clear_all_previous_highwater = this->clear_all_current_highwater;
			this->clear_all_current_highwater = 0;
		}
		else if (this->root && this->root->next == nullptr)
		{
			this->root->alloc_head = (uint8_t*)this->root->mem.ptr;
			this->used_mem = 0;
			this->clear_all_current_highwater = 0;
		}
	}

	bool
	Arena::owns(void* ptr)
	{
		for (auto it = this->root; it != nullptr; it = it->next)
		{
			auto begin_ptr = (char*)it->mem.ptr;
			auto end_ptr = begin_ptr + it->mem.size;
			if (ptr >= begin_ptr && ptr < end_ptr)
				return true;
		}
		return false;
	}
}
