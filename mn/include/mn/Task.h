#pragma once

#include "mn/Memory.h"

#include <utility>
#include <functional>

namespace mn
{
	// a task is a closure wrapper which takes care of allocation, free, and small buffer optimizations
	template<typename>
	struct Task;

	// a task is a closure wrapper which takes care of allocation, free, and small buffer optimizations
	template<typename R, typename ... Args>
	struct Task<R(Args...)>
	{
		struct Concept
		{
			virtual ~Concept() = default;
			virtual R invoke(Args... args) = 0;
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
			{}

			template<typename G>
			Model(Allocator, G&& f)
				:fn(std::forward<G>(f))
			{}

			R invoke(Args... args) override
			{
				return std::invoke(fn, std::forward<Args>(args)...);
			}
		};

		template<typename F>
		struct Model<F, false> final : Concept
		{
			Allocator allocator;
			F* fn;

			template<typename G>
			Model(G&& f)
			{
				allocator = allocator_top();
				fn = alloc_construct_from<F>(allocator, std::forward<G>(f));
			}

			template<typename G>
			Model(Allocator a, G&& f)
			{
				allocator = a;
				fn = alloc_construct_from<F>(allocator, std::forward<F>(f));
			}

			~Model() override
			{
				free_destruct_from(allocator, fn);
			}

			R invoke(Args... args) override
			{
				return std::invoke(*fn, std::forward<Args>(args)...);
			}
		};

		static constexpr size_t SMALL_SIZE = sizeof(void*) * 7;
		alignas(Concept) unsigned char concept[SMALL_SIZE];
		bool isSet;

		Concept& _self()
		{
			return *static_cast<Concept*>(static_cast<void*>(concept));
		}

		R operator()(Args... args)
		{
			return _self().invoke(std::forward<Args>(args)...);
		}

		// bool casting operator to check if the task is holding a closure
		operator bool() const { return isSet; }

		// creates a new empty task
		inline static Task<R(Args...)>
		make()
		{
			Task<R(Args...)> self{};
			return self;
		}

		// creates a new task from the given callable
		template<typename F>
		inline static Task<R(Args...)>
		make(F&& f)
		{
			constexpr bool is_small = sizeof(Model<F, true>) <= SMALL_SIZE;
			Task<R(Args...)> self{};
			::new (&self.concept) Model<F, is_small>(std::forward<F>(f));
			self.isSet = true;
			return self;
		}

		// creates a new task from the given callable using the given allocator
		template<typename F>
		inline static Task<R(Args...)>
		make_with_allocator(Allocator allocator, F&& f)
		{
			constexpr bool is_small = sizeof(Model<F, true>) <= SMALL_SIZE;
			Task<R(Args...)> self{};
			::new (&self.concept) Model<F, is_small>(allocator, std::forward<F>(f));
			self.isSet = true;
			return self;
		}
	};

	// frees the given task
	template<typename R, typename ... Args>
	inline static void
	task_free(Task<R(Args...)>& self)
	{
		if (self.isSet)
		{
			self._self().~Concept();
			self.isSet = false;
		}
	}

	// destruct overload for task free
	template<typename R, typename ... Args>
	inline static void
	destruct(Task<R(Args...)>& self)
	{
		task_free(self);
	}
}
