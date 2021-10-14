#pragma once

#include "mn/Assert.h"

namespace mn
{
	// fixed capacity buffer, with capacity known at compile time
	template<typename T, size_t Capacity>
	struct Fixed_Buf
	{
		size_t count;
		T elements[Capacity];

		T&
		operator[](size_t ix)
		{
			mn_assert(ix < count);
			return elements[ix];
		}

		const T&
		operator[](size_t ix) const
		{
			mn_assert(ix < count);
			return elements[ix];
		}
	};

	// creates a new fixed buffer instance
	template<typename T, size_t Capacity>
	inline static Fixed_Buf<T, Capacity>
	fixed_buf_new()
	{
		Fixed_Buf<T, Capacity> self{};
		self.count = 0;
		return self;
	}

	// frees the given fixed buffer
	template<typename T, size_t Capacity>
	inline static void
	fixed_buf_free(Fixed_Buf<T, Capacity>& self)
	{
		self.count = 0;
	}

	// destruct overload for fixed buffer free
	template<typename T, size_t Capacity>
	inline static void
	destruct(Fixed_Buf<T, Capacity>& self)
	{
		for(size_t i = 0; i < self.count; ++i)
			destruct(self[i]);
		fixed_buf_free(self);
	}

	// pushes a new value to the given buffer
	template<typename T, size_t Capacity>
	inline static void
	fixed_buf_push(Fixed_Buf<T, Capacity>& self, const T& value)
	{
		mn_assert(self.count < Capacity);
		self.elements[self.count] = value;
		++self.count;
	}

	// returns an iterator to the begining of the buffer
	template<typename T, size_t Capacity>
	inline static const T*
	fixed_buf_begin(const Fixed_Buf<T, Capacity>& self)
	{
		return self.elements;
	}

	// returns an iterator to the begining of the buffer
	template<typename T, size_t Capacity>
	inline static T*
	fixed_buf_begin(Fixed_Buf<T, Capacity>& self)
	{
		return self.elements;
	}

	// returns an iterator to the end of the buffer
	template<typename T, size_t Capacity>
	inline static const T*
	fixed_buf_end(const Fixed_Buf<T, Capacity>& self)
	{
		return self.elements + self.count;
	}

	// returns an iterator to the end of the buffer
	template<typename T, size_t Capacity>
	inline static T*
	fixed_buf_end(Fixed_Buf<T, Capacity>& self)
	{
		return self.elements + self.count;
	}

	// begin iterator overload for fixed buffer
	template<typename T, size_t Capacity>
	inline static const T*
	begin(const Fixed_Buf<T, Capacity>& self)
	{
		return fixed_buf_begin(self);
	}

	// begin iterator overload for fixed buffer
	template<typename T, size_t Capacity>
	inline static T*
	begin(Fixed_Buf<T, Capacity>& self)
	{
		return fixed_buf_begin(self);
	}

	// end iterator overload for fixed buffer
	template<typename T, size_t Capacity>
	inline static const T*
	end(const Fixed_Buf<T, Capacity>& self)
	{
		return fixed_buf_end(self);
	}

	// end iterator overload for fixed buffer
	template<typename T, size_t Capacity>
	inline static T*
	end(Fixed_Buf<T, Capacity>& self)
	{
		return fixed_buf_end(self);
	}

}