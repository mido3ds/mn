#pragma once

#include "mn/Memory.h"

#include <type_traits>
#include <functional>

namespace mn
{
	template<typename>
	struct Task;

	template<typename R, typename ... Args>
	struct Task<R(Args...)>
	{
		struct Concept
		{
			Allocator allocator;
			virtual ~Concept() = default;
			virtual R invoke(Args&&... args) = 0;
		};

		template<typename F, bool Small>
		struct Model;

		template<typename F>
		struct Model<F, true> final: Concept
		{
			F fn;

			template<typename G>
			Model(G&& f)
				:fn(std::forward<G>(f))
			{
				allocator = nullptr;
			}

			R invoke(Args&&... args) override
			{
				return std::invoke(fn, std::forward<Args>(args)...);
			}
		};

		template<typename F>
		struct Model<F, false> final : Concept
		{
			F* fn;

			template<typename G>
			Model(G&& f)
			{
				allocator = allocator_top();
				fn = alloc_construct_from<F>(allocator, std::forward<F>(f));
			}

			~Model() override
			{
				free_destruct_from(allocator, fn);
			}

			R invoke(Args&&... args) override
			{
				return std::invoke(*fn, std::forward<Args>(args)...);
			}
		};

		static constexpr size_t SMALL_SIZE = sizeof(void*) * 7;
		Allocator allocator;
		std::aligned_storage_t<SMALL_SIZE> concept;

		Concept& _self()
		{
			return *static_cast<Concept*>(static_cast<void*>(&concept));
		}

		R operator()(Args... args)
		{
			return _self().invoke(std::forward<Args>(args)...);
		}

		template<typename F>
		inline static Task<R(Args...)>
		make(F&& f)
		{
			constexpr bool is_small = sizeof(Model<std::decay_t<F>, true>) <= SMALL_SIZE;
			Task<R(Args...)> self{};
			self.allocator = allocator_top();
			::new (&self.concept) Model<std::decay_t<F>, is_small>(std::forward<F>(f));
			return self;
		}
	};

	template<typename R, typename ... Args>
	inline static void
	task_free(Task<R(Args...)>& self)
	{
		self._self().~Concept();
	}

	template<typename R, typename ... Args>
	inline static void
	destruct(Task<R(Args...)>& self)
	{
		task_free(self);
	}
}
