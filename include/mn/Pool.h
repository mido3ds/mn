#pragma once

#include "mn/Exports.h"
#include "mn/Base.h"
#include "mn/Thread.h"
#include "mn/Memory.h"

namespace mn
{
	/**
	 * Pool handle
	 */
	MS_HANDLE(Pool);

	/**
	 * @brief      Creates a new memory pool
	 * A Memory pool is a list of buckets with `bucket_size` capacity which it uses to hand
	 * over a memory piece with the given `element_size` every time you call `pool_get`
	 *
	 * @param[in]  element_size    The element size (in bytes)
	 * @param[in]  bucket_size     The bucket size (in elements count)
	 * @param[in]  meta_allocator  The meta allocator for the pool to allocator from
	 */
	API_MN Pool
	pool_new(size_t element_size, size_t bucket_size, Allocator meta_allocator = allocator_top());

	/**
	 * @brief      Frees the memory pool
	 *
	 * @param[in]  pool  The pool
	 */
	API_MN void
	pool_free(Pool pool);

	/**
	 * @brief      Destruct function overload for the pool
	 *
	 * @param[in]  pool  The pool
	 */
	inline static void
	destruct(Pool pool)
	{
		pool_free(pool);
	}

	/**
	 * @brief      Returns a memory suitable to write an object of size `element_size` in
	 *
	 * @param[in]  pool  The pool
	 */
	API_MN void*
	pool_get(Pool pool);

	/**
	 * @brief      Puts back the given memory into the pool to be reused later
	 *
	 * @param[in]  pool  The pool
	 * @param      ptr   The pointer to memory
	 */
	API_MN void
	pool_put(Pool pool, void* ptr);


	//sugar coating

	/**
	 * @brief      C++ Sugar coating for the pool
	 *
	 * @tparam     T     Element Type
	 */
	template<typename T>
	struct Typed_Pool
	{
		Pool pool;

		/**
		 * @brief      Constructs a new pool
		 *
		 * @param[in]  bucket_size     The bucket size (in elements count) (optional, default = 1024)
		 * @param[in]  meta_allocator  The meta allocator
		 */
		Typed_Pool(size_t bucket_size = 1024, Allocator meta_allocator = allocator_top())
			:pool(pool_new(sizeof(T), bucket_size, meta_allocator))
		{}

		Typed_Pool(const Typed_Pool& other) = delete;

		Typed_Pool(Typed_Pool&& other) = default;

		Typed_Pool&
		operator=(const Typed_Pool& other) = delete;

		Typed_Pool&
		operator=(Typed_Pool&& other) = default;

		~Typed_Pool()
		{
			destruct(pool);
		}

		/**
		 * @brief      Returns a pointer to the newly allocated element
		 */
		inline T*
		get()
		{
			return (T*)pool_get(pool);
		}

		/**
		 * @brief      Puts back the given pointer into the pool
		 */
		inline void
		put(T* ptr)
		{
			pool_put(pool, ptr);
		}
	};


	/**
	 * @brief      Thread Safe version of the `Typed_Pool`
	 *
	 * @tparam     T     Element Type
	 */
	template<typename T>
	struct TS_Typed_Pool
	{
		Pool pool;
		Mutex mtx;

		/**
		 * @brief      Constructs a new pool
		 *
		 * @param[in]  bucket_size     The bucket size (in elements count) (optional, default = 1024)
		 * @param[in]  meta_allocator  The meta allocator
		 */
		TS_Typed_Pool(size_t bucket_size = 1024, Allocator meta_allocator = allocator_top())
			:pool(pool_new(sizeof(T), bucket_size, meta_allocator)),
			 mtx(mutex_new())
		{}

		TS_Typed_Pool(const TS_Typed_Pool& other) = delete;

		TS_Typed_Pool(TS_Typed_Pool&& other) = default;

		TS_Typed_Pool&
		operator=(const TS_Typed_Pool& other) = delete;

		TS_Typed_Pool&
		operator=(TS_Typed_Pool&& other) = default;

		~TS_Typed_Pool()
		{
			destruct(pool);
			mutex_free(mtx);
		}

		/**
		 * @brief      Returns a pointer to the newly allocated element
		 */
		inline T*
		get()
		{
			T* res = nullptr;
			mutex_lock(mtx);
				res = (T*)pool_get(pool);
			mutex_unlock(mtx);
			return res;
		}

		/**
		 * @brief      Puts back the given pointer into the pool
		 */
		inline void
		put(T* ptr)
		{
			mutex_lock(mtx);
				pool_put(pool, ptr);
			mutex_unlock(mtx);
		}
	};
}