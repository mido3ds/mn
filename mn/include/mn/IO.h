#pragma once
#include "mn/Stream.h"
#include "mn/Reader.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <limits>
#include <assert.h>

namespace mn
{
	/**
	 * [[markdown]]
	 * # The IO
	 * Here's a listing of the function provided by this file
	 * - **print_err**: prints to the stderr
	 * - **println_err**: just like the above but adds a newline at the end
	 * - **printfmt_err**: print a formatted string to the stderr
	 * - **print**: prints to the stdout
	 * - **println**: just like the above but adds a newline at the end
	 * - **printfmt**: print a formatted string to the stdout
	 * - **vprintb**: variadic print binary function that's used to print values as binary data to a specific Stream
	 * - **vprints**: variadic print string function that's used to print values as string data to a specific Stream
	 * - **vprintf**: variadic print formatted string function that's used to print values as string data to a specific Stream
	 * - **read**: reads values as strings from the buffered stdin
	 * - **vreadb**: reads values as binary data from a specific Stream
	 * - **vreads**: reads values as string values from a specific Stream
	 * 
	 * Note that the core functions are the ones prefixed with a `v` like `vprintb`, `vreads` ... etc, other functions just use/wrap them with the stdio handles
	 * 
	 * The print strategy is that you define a `print_str` if you want to print the type as string
	 * or a `print_bin` if you want to print it as a binary
	 * We already define a `print_bin`, `print_str`, `read_bin`, and `read_str` for the basic types
	 * 
	 * ## Custom Print String Function
	 * ```C++
	 * inline static usize
	 * print_str(Stream stream, Print_Format& format, const Your_Type& value);
	 * ```
	 * If you define this function then it will used to print your type by the `vprints` or `vprintf` function
	 * As you may notice if it's called from `vprintf` function then the format style will be filled and passed along for you to do the necessary formatting of your type
	 * If it's called from `vprints` then the format will be default initialized
	 * You can have a look at the `Print_Format` structure
	 * 
	 * ## Custom Print Binary Function
	 * ```C++
	 * inline static usize
	 * print_bin(Stream stream, const Your_Type& value)
	 * {
	 * 	return stream_write(stream, block_from(value));
	 * }
	 * ```
	 * If you define this function then it will be used when you use the `vprintb` function
	 * The default behaviour for your custom types is that we just dump the binary representation of it in memory to the IO_Trait so if that suits you then there's no need to provide a custom function
	 * 
	 * ## Custom Read String Function
	 * ```C++
	 * inline static usize
	 * read_str(Reader reader, Your_Type& value);
	 * ```
	 * If you define this function then it will be used when you use the `vreads` function
	 * 
	 * ## Custom Read Binary Function
	 * ```C++
	 * inline static usize
	 * read_bin(Stream stream, Your_Type& value)
	 * 
	 * inline static usize
	 * read_bin(Reader reader, Your_Type& value)
	 * ```
	 * If you define this function then it will be used when you use the `vreadb` function
	 * The default behaviour for your custom types is that we just read the `sizeof(Your_Type)` from the IO_Trait so if that suits you then there's no need to provide a custom function
	 * 
	 * ## Formatted Print Documentation
	 * The print formatted string has a simple grammar to specify different styles of printing values
	 * 
	 * `vprintf(stream, "Normal String");`
	 * You can just print a normal string
	 * 
	 * 
	 * `vprintf(stream, "Formatted String {}", value);`
	 * You can specify the placeholder of a value using the `{}` where you want the values to be printed
	 * 
	 * 
	 * `vprintf(stream, "My Name is {}, and my age is {}", "Mostafa", 25);`
	 * You can have multiple values and send them as arguments to the function and they will be used in the same order producing the result of `My Name is Mostafa, and my age is 25`
	 * 
	 * 
	 * `vprintf(stream, "My Name is {0}, and my age is {1}, Hello {0}", "Mostafa", 25);`
	 * You can specify the index of the variable to be printed in the location by using its index so the above line prints `My Name is Mostafa, and my age is 25, Hello Mostafa`
	 * 
	 * 
	 * Note that you cannot mix unindexed placeholder and indexed placeholders in the same print statement i.e. `My Name is {0}, and my age is {}, Hello {0}` This will result in an error
	 * 
	 * ### The Anatomy of a placeholder
	 * `{}`
	 * This is the simplist placeholder that could ever exists and it's un-indexed
	 * 
	 * `{0}`
	 * This is an indexed placeholder
	 * 
	 * `{0:_<+#20.5f}`
	 * This is the most complex placeholder
	 * So the anatomy is simple there's a left part which only contains the index which can be optional and then there's a colon then there's some style specifiers
	 * - `_`: this is the padding letter to be used to fill empty spaces so in this case it's `_` but it can be anything you like
	 * - `<`: this is the alignment specifier which in this case do a left alignment of the text according to the specified width
	 * 	- `<`: left alignment
	 * 	- `>`: right alignment
	 * 	- `^`: center alignment
	 * - `+`: this is the sign spcifier which in this case prints the sign of the number always
	 * 	- `+`: always prints the sign of the number so it prints `+` for positive and `-` for negative numbers
	 * 	- `-`: prints the sign of the negative numbers only and this is the default
	 * 	- ` `: a space prints the sign of the negative numbers only and prints an empty space for the positive numbers
	 * - `#`: this is the prefix specifier which print the `0x` prefix in the case of the hex values and `0o` in case of octal values
	 * - `20`: this is the width specifier which specifies the width of this placeholders in letters so in this case this placeholder has a 20 letter width and this will be used in things like the alignement ... etc. this can be any number you like.
	 * - `.5`: this is the precision specifiers which is valid only with floating point values and it specifies that we should print to the 5th place after the `.` and this can be any number you like i.e. `.2`, `.10`, ... etc.
	 * - `f`: this is the type formatter in this case it tells the format to treat the value as a floating point number
	 * 	- `c`: this specifies the value to be printed as a letter
	 * 	- `d`: this specifies the value to be printed as a decimal number
	 * 	- `o`: this specifies the value to be printed as an octal number
	 * 	- `x`: this specifies the value to be printed as a hex number with small letters `0xabcdef`
	 * 	- `X`: this specifies the value to be printed as a hex number with capital letters `0xABCDEF`
	 * 	- `e`: this is a floating point number only format which prints the value in scientific exp format using the small `e` letter
	 * 	- `E`: this is a floating point number only format which prints the value in scientific exp format using the capital `E` letter
	 * 	- `f`: this is a floating point number only format which prints the value in floating point style `1.50` the default precision is 6 digits for `r32` and 6 digits for `r64`
	 * 	- `g`: this is a floating point number only format which prints floating-point number to decimal or decimal exponent notation using a small letter `e` depending on the value and the precision. i.e. if the value is `1.230000` when printed with `f` specifier then it will be `1.23` with the `g` specifier, similarly `1.000000` with the `f` will be `1` with the `g`
	 * 	- `G`: this is a floating point number only format which prints floating-point number to decimal or decimal exponent notation using a capital letter `E` depending on the value and the precision. i.e. if the value is `1.230000` when printed with `f` specifier then it will be `1.23` with the `G` specifier, similarly `1.000000` with the `f` will be `1` with the `G`
	 * 
	 * 
	 * Please note that this is only a format request so the `print_str` may choose to not respect those requests if they are invalid or meaningless to the value i.e. `f` specifier with an integer value
	 * You can print `{` curly braces by using the `{{` in the format string but you don't need to escape the `}`
	 * ##Examples
	 * Input:
	 * ```C++
	 * printfmt("Hello, my name is {}, my age is {:.2f}", "Mostafa", 25.0f);
	 * ```
	 * Output:
	 * ```
	 * Hello, my name is Mostafa, my age is 25.00
	 * ```
	 * 
	 * Input:
	 * ```C++
	 * for(usize i = 0; i < 10; ++i)
	 * 	printfmt("My Random number is {:_^+10}\n", rand());
	 * ```
	 * Output:
	 * ```
	 * My Random number is +___41____
	 * My Random number is +__18467__
	 * My Random number is +__6334___
	 * My Random number is +__26500__
	 * My Random number is +__19169__
	 * My Random number is +__15724__
	 * My Random number is +__11478__
	 * My Random number is +__29358__
	 * My Random number is +__26962__
	 * My Random number is +__24464__
	 * ```
	 * 
	 * Input:
	 * ```C++
	 * printfmt("Person {{ Name: {}, Age: {} }\n", "Mostafa", 25);
	 * ```
	 * Output:
	 * ```
	 * Person { Name: Mostafa, Age: 25 }
	 * ```
	 */

	struct Print_Format
	{
		/**
		 * @brief      Alignment of the printed string value
		 * 
		 * - **NONE**: The invalid alignment
		 * - **LEFT**: '<' left aligns the value
		 * - **RIGHT**: '>' right aligns the value
		 * - **CENTER**: '^' center aligns the value
		 * - **EQUAL**: '=' used with numbers only for printing values like +000120
		 */
		enum class ALIGN
		{
			NONE,
			LEFT,
			RIGHT,
			CENTER,
			EQUAL
		};

		/**
		 * @brief      Sign style of the printed numbers
		 * 
		 * - **NONE**: The invalid sign
		 * - **POSITIVE**: always print signs before the number
		 * - **NEGATIVE**: print only negative signs
		 * - **SPACE**: print one space for positive numbers
		 */
		enum class SIGN
		{
			NONE,
			POSITIVE,
			NEGATIVE,
			SPACE
		};

		/**
		 * @brief      The type style of the printed value
		 * 
		 * - **NONE**: The invalid type
		 * - **BINARY**: print in binary format
		 * - **RUNE**: print as a rune
		 * - **DECIMAL**: print as decimal value
		 * - **OCTAL**: print as octal value
		 * - **HEX_SMALL**: print as a hexadecimal value with small 'abcdef' letters
		 * - **HEX_CAPITAL**: print as a hexadecimal value with capital `ABCDEF` letters
		 * - The following options are for floating point only
		 * - **EXP_SMALL**: print floats as exponent format with small case 'e'
		 * - **EXP_CAPITAL**: print floats as exponent format with capital case 'E'
		 * - **FLOAT**: print as floating point value
		 * - **GENERAL_SMALL**: converts floating point number to decimal or decimal small exponent notation depending on the value and precision
		 * - **GENERAL_CAPITAL**: converts floating point number to decimal or decimal capital exponent notation depending on the value and precision
		 */
		enum class TYPE
		{
			NONE,
			BINARY,
			RUNE,
			DECIMAL,
			OCTAL,
			HEX_SMALL,
			HEX_CAPITAL,

			//float only types
			EXP_SMALL,
			EXP_CAPITAL,
			FLOAT,
			GENERAL_SMALL,
			GENERAL_CAPITAL
		};

		size_t index;
		ALIGN alignment;
		int32_t pad;
		SIGN  sign;
		bool  prefix;
		size_t width;
		size_t precision;
		TYPE type;

		Print_Format()
			:index(0),
			alignment(ALIGN::NONE),
			pad(' '),
			sign(SIGN::NONE),
			prefix(false),
			width(0),
			precision(static_cast<size_t>(-1)),
			type(TYPE::NONE)
		{}
	};

	inline static void
	_variadic_print_binary_helper(Stream, size_t&)
	{
		return;
	}

	template<typename TFirst, typename ... TArgs>
	inline static void
	_variadic_print_binary_helper(Stream stream,
								  size_t& size,
								  TFirst&& first_arg,
								  TArgs&& ... args)
	{
		size += print_bin(stream, std::forward<TFirst>(first_arg));
		_variadic_print_binary_helper(stream, size, std::forward<TArgs>(args)...);
	}

	template<typename T>
	inline static size_t
	_generic_print_str_function(void* _self, Print_Format& format, Stream stream)
	{
		T* value = (T*)_self;
		return print_str(stream, format, *value);
	}

	struct Generic_Print_Str_Value
	{
		using print_func = size_t(*)(void*, Print_Format&, Stream stream);
		void* _self;
		print_func _print;

		template<typename T>
		Generic_Print_Str_Value(const T& value)
		{
			_self = (void*)(&value);
			_print = _generic_print_str_function<T>;
		}

		size_t
		print(Stream stream, Print_Format& format)
		{
			return _print(_self, format, stream);
		}
	};

	inline static void
	_variadic_print_string_helper(Stream, size_t&)
	{
		return;
	}

	template<typename TFirst, typename ... TArgs>
	inline static void
	_variadic_print_string_helper(Stream stream,
								  size_t& size,
								  TFirst&& first_arg,
								  TArgs&& ... args)
	{
		Print_Format format;
		size += print_str(stream, format, std::forward<TFirst>(first_arg));
		_variadic_print_string_helper(stream, size, std::forward<TArgs>(args)...);
	}

	inline static bool
	_is_digit(int32_t c)
	{
		return (c == '0' ||
				c == '1' ||
				c == '2' ||
				c == '3' ||
				c == '4' ||
				c == '5' ||
				c == '6' ||
				c == '7' ||
				c == '8' ||
				c == '9');
	}

	inline static bool
	_is_type(int32_t c)
	{
		return (c == 'b' ||
				c == 'c' ||
				c == 'd' ||
				c == 'o' ||
				c == 'x' ||
				c == 'X' ||
				c == 'e' ||
				c == 'E' ||
				c == 'f' ||
				c == 'g' ||
				c == 'G');
	}

	inline static size_t
	_parse_value_index(const char*& it, const char* end, bool &error)
	{
		error = true;
		size_t result = 0;

		while(it != end)
		{
			auto c = *it;
			if(!_is_digit(c))
			{
				return result;
			}
			else
			{
				result *= 10;
				result += c - '0';
				error = false;
			}
			++it;
		}

		return result;
	}

	inline static void
	_parse_format(const char*& it,
				  const char* end,
				  Print_Format& format,
				  bool& manual_indexing)
	{
		int32_t c = *it;
		//user defined index
		if (_is_digit(c))
		{
			bool err;
			format.index = _parse_value_index(it, end, err);
			assert(err == false &&
				   "invalid index in vprintf statement");
			manual_indexing = true;
		}
		else
		{
			assert(manual_indexing == false &&
				   "cannot mix manual indexing with automatic indexing in a vprintf statement");
		}

		//check the colon
		c = *it;
		if (c != ':')
		{
			//there's no format specifiers
			//then we should end the format with a }
			if (c != '}')
			{
				assert(false &&
					   "missing } in format specifiers in a vprintf statement");
			}
			//we found the } then we increment
			else
			{
				++it;
			}
			return;
		}
		++it;

		auto after_colon = it;
		++it;

		c = *it;
		//check the alignment
		if (c == '<')
		{
			format.alignment = Print_Format::ALIGN::LEFT;
			format.pad = *after_colon;
			++it;
		}
		else if (c == '>')
		{
			format.alignment = Print_Format::ALIGN::RIGHT;
			format.pad = *after_colon;
			++it;
		}
		else if (c == '^')
		{
			format.alignment = Print_Format::ALIGN::CENTER;
			format.pad = *after_colon;
			++it;
		}
		else
		{
			it = after_colon;
			c = *it;
			if (c == '<')
			{
				format.alignment = Print_Format::ALIGN::LEFT;
				++it;
			}
			else if (c == '>')
			{
				format.alignment = Print_Format::ALIGN::RIGHT;
				++it;
			}
			else if (c == '^')
			{
				format.alignment = Print_Format::ALIGN::CENTER;
				++it;
			}
		}

		c = *it;
		//check the sign
		if (c == '+')
		{
			format.sign = Print_Format::SIGN::POSITIVE;
			++it;
		}
		else if (c == '-')
		{
			format.sign = Print_Format::SIGN::NEGATIVE;
			++it;
		}
		else if (c == ' ')
		{
			format.sign = Print_Format::SIGN::SPACE;
			++it;
		}

		c = *it;
		//check the #
		if (c == '#')
		{
			format.prefix = true;
			++it;
		}

		c = *it;
		//check the width
		if (_is_digit(c))
		{
			bool err;
			format.width = _parse_value_index(it, end, err);
			assert(err == false &&
				   "invalid width in vprintf statement");
		}

		c = *it;
		//check the precision
		if (c == '.')
		{
			++it;
			bool err;
			format.precision = _parse_value_index(it, end, err);
			assert(err == false &&
				   "invalid precision in vprintf statement");
		}

		c = *it;
		if (c == 'c')
		{
			format.type = Print_Format::TYPE::RUNE;
			++it;
		}
		else if (c == 'd')
		{
			format.type = Print_Format::TYPE::DECIMAL;
			++it;
		}
		else if(c == 'b')
		{
			format.type = Print_Format::TYPE::BINARY;
			++it;
		}
		else if (c == 'o')
		{
			format.type = Print_Format::TYPE::OCTAL;
			++it;
		}
		else if (c == 'x')
		{
			format.type = Print_Format::TYPE::HEX_SMALL;
			++it;
		}
		else if (c == 'X')
		{
			format.type = Print_Format::TYPE::HEX_CAPITAL;
			++it;
		}
		else if (c == 'e')
		{
			format.type = Print_Format::TYPE::EXP_SMALL;
			++it;
		}
		else if (c == 'E')
		{
			format.type = Print_Format::TYPE::EXP_CAPITAL;
			++it;
		}
		else if (c == 'f')
		{
			format.type = Print_Format::TYPE::FLOAT;
			++it;
		}
		else if (c == 'g')
		{
			format.type = Print_Format::TYPE::GENERAL_SMALL;
			++it;
		}
		else if (c == 'G')
		{
			format.type = Print_Format::TYPE::GENERAL_CAPITAL;
			++it;
		}

		c = *it;
		//check the end brace
		if(c != '}')
		{
			assert(false &&
				   "missing } in format specifiers in a vprintf statement");
		}

		++it;
		return;
	}

	template<typename T, size_t BUFFER_SIZE>
	inline static size_t
	_print_integer(Stream stream, Print_Format& format, const char* pattern, T value)
	{
		size_t result = 0;

		//respect the sign
		if (value >= 0 &&
			format.type == Print_Format::TYPE::DECIMAL &&
			format.sign == Print_Format::SIGN::POSITIVE)
		{
			char sign = '+';
			result += stream_write(stream, block_from(sign));
		}
		else if(value >= 0 &&
				format.type == Print_Format::TYPE::DECIMAL &&
				format.sign == Print_Format::SIGN::SPACE)
		{
			char sign = ' ';
			result += stream_write(stream, block_from(sign));
		}

		//render the value and calc it's written size
		char buffer[BUFFER_SIZE];
		int written_size = ::snprintf(buffer, BUFFER_SIZE, pattern, value);
		if(written_size < 0)
			return 0;

		//respect right and center align
		size_t pad_size = 0, pad_str_size = 0;
		Print_Format::ALIGN alignment = format.alignment;
		if(format.width > (written_size + result))
		{
			pad_size = format.width - (written_size + result);
			pad_str_size = ::strlen((char*)&format.pad);
			//default alignment is LEFt
			if(alignment == Print_Format::ALIGN::NONE)
				alignment = Print_Format::ALIGN::LEFT;
		}

		if (pad_size != 0 &&
			alignment == Print_Format::ALIGN::RIGHT)
		{
			for(size_t i = 0; i < pad_size; ++i)
				result += stream_write(stream, Block { (void*)&format.pad, pad_str_size });
		}
		else if(pad_size != 0 &&
				alignment == Print_Format::ALIGN::CENTER)
		{
			size_t i;
			for(i = 0; i < pad_size / 2; ++i)
				result += stream_write(stream, Block { (void*)&format.pad, pad_str_size });
			pad_size -= i;
		}

		//write the rendered value
		result += stream_write(stream, Block { buffer, size_t(written_size) });

		//respect left and center align
		if (pad_size != 0 &&
			alignment == Print_Format::ALIGN::CENTER)
		{
			for(size_t i = 0; i < pad_size; ++i)
				result += stream_write(stream, Block { (void*)&format.pad, pad_str_size });
		}
		else if(pad_size != 0 &&
				alignment == Print_Format::ALIGN::LEFT)
		{
			for(size_t i = 0; i < pad_size; ++i)
				result += stream_write(stream, Block { (void*)&format.pad, pad_str_size });
		}

		return result;
	}

	template<typename T, size_t BUFFER_SIZE, size_t PERCISION>
	inline static size_t
	_print_real(Stream stream, Print_Format& _format, T value)
	{
		auto format = _format;

		//first respect the pattern
		const char* PATTERN = nullptr;
		switch(format.type)
		{
			case Print_Format::TYPE::EXP_SMALL:
				PATTERN = "%.*e";
				break;
			case Print_Format::TYPE::EXP_CAPITAL:
				PATTERN = "%.*E";
				break;
			case Print_Format::TYPE::FLOAT:
				PATTERN = "%.*f";
				break;
			case Print_Format::TYPE::GENERAL_SMALL:
				PATTERN = "%.*g";
				break;
			case Print_Format::TYPE::GENERAL_CAPITAL:
			default:
				PATTERN = "%.*G";
				break;
		}

		size_t result = 0;

		//second respect the sign
		if (value >= 0 &&
			format.sign == Print_Format::SIGN::POSITIVE)
		{
			char s = '+';
			result += stream_write(stream, block_from(s));
		}
		else if(value >= 0 &&
				format.sign == Print_Format::SIGN::SPACE)
		{
			char s = ' ';
			result += stream_write(stream, block_from(s));
		}

		if(format.precision == static_cast<size_t>(-1))
			format.precision = PERCISION;

		//third render the value and calc it's written size
		char buffer[BUFFER_SIZE];
		int written_size = ::snprintf(buffer, BUFFER_SIZE, PATTERN, format.precision, value);
		if(written_size < 0) return 0;

		//fourth respect right and center align
		size_t pad_size = 0, pad_str_size = 0;
		if(format.width > (written_size + result))
		{
			pad_size = format.width - (written_size + result);
			pad_str_size = ::strlen((char*)&format.pad);
			//default alignment is LEFT
			if (format.alignment == Print_Format::ALIGN::NONE)
				format.alignment = Print_Format::ALIGN::LEFT;
		}

		if (pad_size != 0 &&
			format.alignment == Print_Format::ALIGN::RIGHT)
		{
			for(size_t i = 0; i < pad_size; ++i)
				result += stream_write(stream, Block { (void*)&format.pad, pad_str_size });
		}
		else if(pad_size != 0 &&
				format.alignment == Print_Format::ALIGN::CENTER)
		{
			size_t i;
			for(i = 0; i < pad_size / 2; ++i)
				result += stream_write(stream, Block { (void*)&format.pad, pad_str_size });
			pad_size -= i;
		}

		//fifth write the rendered value
		result += stream_write(stream, Block { buffer, size_t(written_size) });

		//sixth respect left and center align
		if (pad_size != 0 &&
			format.alignment == Print_Format::ALIGN::CENTER)
		{
			for(size_t i = 0; i < pad_size; ++i)
				result += stream_write(stream, Block { (void*)&format.pad, pad_str_size });
		}
		else if(pad_size != 0 &&
				format.alignment == Print_Format::ALIGN::LEFT)
		{
			for(size_t i = 0; i < pad_size; ++i)
				result += stream_write(stream, Block { (void*)&format.pad, pad_str_size });
		}

		return result;
	}

	template<typename T>
	inline static size_t
	print_bin(Stream stream, const T& value)
	{
		return stream_write(stream, block_from(value));
	}

	template<typename ... TArgs>
	inline static size_t
	vprintb(Stream stream, TArgs&& ... args)
	{
		size_t result = 0;
		_variadic_print_binary_helper(stream, result, std::forward<TArgs>(args)...);
		return result;
	}

	inline static size_t
	print_str(Stream stream, Print_Format& format, char value)
	{
		//first respect the pattern
		const char* PATTERN = nullptr;
		switch(format.type)
		{
			case Print_Format::TYPE::DECIMAL:
				PATTERN = "%hhd";
				break;
			case Print_Format::TYPE::HEX_SMALL:
				if(format.prefix)
					PATTERN = "%#hhx";
				else
					PATTERN = "%hhx";
				break;
			case Print_Format::TYPE::HEX_CAPITAL:
				if(format.prefix)
					PATTERN = "%#hhX";
				else
					PATTERN = "%hhX";
				break;
			case Print_Format::TYPE::OCTAL:
				if(format.prefix)
					PATTERN = "%#hho";
				else
					PATTERN = "%hho";
				break;
			case Print_Format::TYPE::RUNE:
			case Print_Format::TYPE::NONE:
			default:
				PATTERN = "%c";
				break;
		}

		return _print_integer<char, 6>(stream,
									   format,
									   PATTERN,
									   value);
	}

	inline static size_t
	print_str(Stream stream, Print_Format& _format, int8_t value)
	{
		auto format = _format;
		//first respect the pattern
		const char* PATTERN = nullptr;
		switch(format.type)
		{
			case Print_Format::TYPE::RUNE:
				PATTERN = "%c";
				break;
			case Print_Format::TYPE::HEX_SMALL:
				if(format.prefix)
					PATTERN = "%#hhx";
				else
					PATTERN = "%hhx";
				break;
			case Print_Format::TYPE::HEX_CAPITAL:
				if(format.prefix)
					PATTERN = "%#hhX";
				else
					PATTERN = "%hhX";
				break;
			case Print_Format::TYPE::OCTAL:
				if(format.prefix)
					PATTERN = "%#hho";
				else
					PATTERN = "%hho";
				break;
			case Print_Format::TYPE::DECIMAL:
			case Print_Format::TYPE::NONE:
			default:
				PATTERN = "%hhd";
				format.type = Print_Format::TYPE::DECIMAL;
				break;
		}

		return _print_integer<int8_t, 6>(stream,
										 format,
										 PATTERN,
										 value);
	}

	inline static size_t
	print_str(Stream stream, Print_Format& _format, uint8_t value)
	{
		auto format = _format;
		//first respect the pattern
		const char* PATTERN = nullptr;
		switch(format.type)
		{
			case Print_Format::TYPE::RUNE:
				PATTERN = "%c";
				break;
			case Print_Format::TYPE::HEX_SMALL:
				if(format.prefix)
					PATTERN = "%#hhx";
				else
					PATTERN = "%hhx";
				break;
			case Print_Format::TYPE::HEX_CAPITAL:
				if(format.prefix)
					PATTERN = "%#hhX";
				else
					PATTERN = "%hhX";
				break;
			case Print_Format::TYPE::OCTAL:
				if(format.prefix)
					PATTERN = "%#hho";
				else
					PATTERN = "%hho";
				break;
			case Print_Format::TYPE::DECIMAL:
			case Print_Format::TYPE::NONE:
			default:
				PATTERN = "%hhu";
				format.type = Print_Format::TYPE::DECIMAL;
				break;
		}

		return _print_integer<uint8_t, 6>(stream,
										  format,
										  PATTERN,
										  value);
	}

	inline static size_t
	print_str(Stream stream, Print_Format& _format, int16_t value)
	{
		auto format = _format;
		//first respect the pattern
		const char* PATTERN = nullptr;
		switch(format.type)
		{
			case Print_Format::TYPE::RUNE:
				PATTERN = "%c";
				break;
			case Print_Format::TYPE::HEX_SMALL:
				if(format.prefix)
					PATTERN = "%#hx";
				else
					PATTERN = "%hx";
				break;
			case Print_Format::TYPE::HEX_CAPITAL:
				if(format.prefix)
					PATTERN = "%#hX";
				else
					PATTERN = "%hX";
				break;
			case Print_Format::TYPE::OCTAL:
				if(format.prefix)
					PATTERN = "%#ho";
				else
					PATTERN = "%ho";
				break;
			case Print_Format::TYPE::DECIMAL:
			case Print_Format::TYPE::NONE:
			default:
				PATTERN = "%hd";
				format.type = Print_Format::TYPE::DECIMAL;
				break;
		}

		return _print_integer<int16_t, 10>(stream,
										   format,
										   PATTERN,
										   value);
	}

	inline static size_t
	print_str(Stream stream, Print_Format& _format, uint16_t value)
	{
		auto format = _format;
		//first respect the pattern
		const char* PATTERN = nullptr;
		switch(format.type)
		{
			case Print_Format::TYPE::RUNE:
				PATTERN = "%c";
				break;
			case Print_Format::TYPE::HEX_SMALL:
				if(format.prefix)
					PATTERN = "%#hx";
				else
					PATTERN = "%hx";
				break;
			case Print_Format::TYPE::HEX_CAPITAL:
				if(format.prefix)
					PATTERN = "%#hX";
				else
					PATTERN = "%hX";
				break;
			case Print_Format::TYPE::OCTAL:
				if(format.prefix)
					PATTERN = "%#ho";
				else
					PATTERN = "%ho";
				break;
			case Print_Format::TYPE::DECIMAL:
			case Print_Format::TYPE::NONE:
			default:
				PATTERN = "%hu";
				format.type = Print_Format::TYPE::DECIMAL;
				break;
		}

		return _print_integer<uint16_t, 10>(stream,
											format,
											PATTERN,
											value);
	}

	inline static size_t
	print_str(Stream stream, Print_Format& _format, int32_t value)
	{
		auto format = _format;
		//first respect the pattern
		const char* PATTERN = nullptr;
		switch(format.type)
		{
			case Print_Format::TYPE::RUNE:
				PATTERN = "%lc";
				break;
			case Print_Format::TYPE::HEX_SMALL:
				if(format.prefix)
					PATTERN = "%#x";
				else
					PATTERN = "%x";
				break;
			case Print_Format::TYPE::HEX_CAPITAL:
				if(format.prefix)
					PATTERN = "%#X";
				else
					PATTERN = "%X";
				break;
			case Print_Format::TYPE::OCTAL:
				if(format.prefix)
					PATTERN = "%#o";
				else
					PATTERN = "%o";
				break;
			case Print_Format::TYPE::DECIMAL:
			case Print_Format::TYPE::NONE:
			default:
				PATTERN = "%d";
				format.type = Print_Format::TYPE::DECIMAL;
				break;
		}

		return _print_integer<int32_t, 20>(stream,
										   format,
										   PATTERN,
										   value);
	}

	inline static size_t
	print_str(Stream stream, Print_Format& _format, uint32_t value)
	{
		auto format = _format;
		//first respect the pattern
		const char* PATTERN = nullptr;
		switch(format.type)
		{
			case Print_Format::TYPE::RUNE:
				PATTERN = "%c";
				break;
			case Print_Format::TYPE::HEX_SMALL:
				if(format.prefix)
					PATTERN = "%#x";
				else
					PATTERN = "%x";
				break;
			case Print_Format::TYPE::HEX_CAPITAL:
				if(format.prefix)
					PATTERN = "%#X";
				else
					PATTERN = "%X";
				break;
			case Print_Format::TYPE::OCTAL:
				if(format.prefix)
					PATTERN = "%#o";
				else
					PATTERN = "%o";
				break;
			case Print_Format::TYPE::DECIMAL:
			case Print_Format::TYPE::NONE:
			default:
				PATTERN = "%u";
				format.type = Print_Format::TYPE::DECIMAL;
				break;
		}

		return _print_integer<uint32_t, 20>(stream,
											format,
											PATTERN,
											value);
	}

	inline static size_t
	print_str(Stream stream, Print_Format& _format, int64_t value)
	{
		auto format = _format;
		//first respect the pattern
		const char* PATTERN = nullptr;
		#if defined(OS_WINDOWS)
		{
			switch(format.type)
			{
				case Print_Format::TYPE::RUNE:
					PATTERN = "%c";
					break;
				case Print_Format::TYPE::HEX_SMALL:
					if(format.prefix)
						PATTERN = "%#llx";
					else
						PATTERN = "%llx";
					break;
				case Print_Format::TYPE::HEX_CAPITAL:
					if(format.prefix)
						PATTERN = "%#llX";
					else
						PATTERN = "%llX";
					break;
				case Print_Format::TYPE::OCTAL:
					if(format.prefix)
						PATTERN = "%#llo";
					else
						PATTERN = "%llo";
					break;
				case Print_Format::TYPE::DECIMAL:
				case Print_Format::TYPE::NONE:
				default:
					PATTERN = "%lld";
					format.type = Print_Format::TYPE::DECIMAL;
					break;
			}
		}
		#elif defined(OS_LINUX)
		{
			switch(format.type)
			{
				case Print_Format::TYPE::RUNE:
					PATTERN = "%c";
					break;
				case Print_Format::TYPE::HEX_SMALL:
					if(format.prefix)
						PATTERN = "%#lx";
					else
						PATTERN = "%lx";
					break;
				case Print_Format::TYPE::HEX_CAPITAL:
					if(format.prefix)
						PATTERN = "%#lX";
					else
						PATTERN = "%lX";
					break;
				case Print_Format::TYPE::OCTAL:
					if(format.prefix)
						PATTERN = "%#lo";
					else
						PATTERN = "%lo";
					break;
				case Print_Format::TYPE::DECIMAL:
				case Print_Format::TYPE::NONE:
				default:
					PATTERN = "%ld";
					format.type = Print_Format::TYPE::DECIMAL;
					break;
			}
		}
		#endif

		return _print_integer<int64_t, 40>(stream,
										   format,
										   PATTERN,
										   value);
	}

	inline static size_t
	print_str(Stream stream, Print_Format& _format, uint64_t value)
	{
		auto format = _format;
		//first respect the pattern
		const char* PATTERN = nullptr;
		#if defined(OS_WINDOWS)
		{
			switch(format.type)
			{
				case Print_Format::TYPE::RUNE:
					PATTERN = "%c";
					break;
				case Print_Format::TYPE::HEX_SMALL:
					if(format.prefix)
						PATTERN = "%#llx";
					else
						PATTERN = "%llx";
					break;
				case Print_Format::TYPE::HEX_CAPITAL:
					if(format.prefix)
						PATTERN = "%#llX";
					else
						PATTERN = "%llX";
					break;
				case Print_Format::TYPE::OCTAL:
					if(format.prefix)
						PATTERN = "%#llo";
					else
						PATTERN = "%llo";
					break;
				case Print_Format::TYPE::DECIMAL:
				case Print_Format::TYPE::NONE:
				default:
					PATTERN = "%llu";
					format.type = Print_Format::TYPE::DECIMAL;
					break;
			}
		}
		#elif defined(OS_LINUX)
		{
			switch(format.type)
			{
				case Print_Format::TYPE::RUNE:
					PATTERN = "%c";
					break;
				case Print_Format::TYPE::HEX_SMALL:
					if(format.prefix)
						PATTERN = "%#lx";
					else
						PATTERN = "%lx";
					break;
				case Print_Format::TYPE::HEX_CAPITAL:
					if(format.prefix)
						PATTERN = "%#lX";
					else
						PATTERN = "%lX";
					break;
				case Print_Format::TYPE::OCTAL:
					if(format.prefix)
						PATTERN = "%#lo";
					else
						PATTERN = "%lo";
					break;
				case Print_Format::TYPE::DECIMAL:
				case Print_Format::TYPE::NONE:
				default:
					PATTERN = "%lu";
					format.type = Print_Format::TYPE::DECIMAL;
					break;
			}
		}
		#endif

		return _print_integer<uint64_t, 40>(stream,
											format,
											PATTERN,
											value);
	}

	inline static size_t
	print_str(Stream stream, Print_Format& _format, float value)
	{
		return _print_real<float, 128, 6>(stream, _format, value);
	}

	inline static size_t
	print_str(Stream stream, Print_Format& _format, double value)
	{
		return _print_real<double, 265, 12>(stream, _format, value);
	}

	inline static size_t
	print_str(Stream stream, Print_Format& format, void* ptr)
	{
		return print_str(stream, format, size_t(ptr));
	}

	inline static size_t
	print_str(Stream stream, Print_Format& format, const Str& str)
	{
		size_t result = 0;
		//respect right and center align
		size_t written_size = str.count;
		size_t pad_size = 0, pad_str_size = 0;
		Print_Format::ALIGN alignment = format.alignment;
		if(format.width > (written_size + result))
		{
			pad_size = format.width - (written_size + result);
			pad_str_size = ::strlen((char*)&format.pad);
			//default alignment is LEFt
			if(alignment == Print_Format::ALIGN::NONE)
				alignment = Print_Format::ALIGN::LEFT;
		}

		if (pad_size != 0 &&
			alignment == Print_Format::ALIGN::RIGHT)
		{
			for(size_t i = 0; i < pad_size; ++i)
				result += stream_write(stream, Block { (void*)&format.pad, pad_str_size });
		}
		else if(pad_size != 0 &&
				alignment == Print_Format::ALIGN::CENTER)
		{
			size_t i;
			for(i = 0; i < pad_size / 2; ++i)
				result += stream_write(stream, Block { (void*)&format.pad, pad_str_size });
			pad_size -= i;
		}

		result += stream_write(stream, Block { str.ptr, str.count });

		//respect left and center align
		if (pad_size != 0 &&
			alignment == Print_Format::ALIGN::CENTER)
		{
			for(size_t i = 0; i < pad_size; ++i)
				result += stream_write(stream, Block { (void*)&format.pad, pad_str_size });
		}
		else if(pad_size != 0 &&
				alignment == Print_Format::ALIGN::LEFT)
		{
			for(size_t i = 0; i < pad_size; ++i)
				result += stream_write(stream, Block { (void*)&format.pad, pad_str_size });
		}

		return result;
	}

	inline static size_t
	print_str(Stream stream, Print_Format& format, const char* str)
	{
		return print_str(stream, format, str_lit(str));
	}

	template<typename ... TArgs>
	inline static size_t
	vprints(Stream stream, TArgs&& ... args)
	{
		size_t result = 0;
		_variadic_print_string_helper(stream, result, std::forward<TArgs>(args)...);
		return result;
	}

	inline static size_t
	vprintf(Stream stream, const char* str)
	{
		Print_Format format;
		return print_str(stream, format, str);
	}

	template<typename ... TArgs>
	inline static size_t
	vprintf(Stream stream, const char* str_format, TArgs&& ... args)
	{
		size_t result = 0;
		Generic_Print_Str_Value values[] = { args... };
		//size of the variadic template
		size_t values_count = sizeof...(args);
		//default index for the {default_index}
		size_t default_index = 0;
		//switched to manual indexing
		bool manual_indexing = false;

		//we go forward until we encounter a { then we write
		//the chunk defined by [rune_back_it, rune_forward_it)
		auto rune_forward_it = str_format;
		auto rune_back_it = rune_forward_it;
		auto rune_end = str_format + ::strlen(str_format);

		//loop through the str_format string
		while(rune_forward_it != rune_end)
		{
			int32_t c = *rune_forward_it;

			//if this is a { then this maybe an argument
			if(c == '{')
			{
				//if there's some chunk to be written then we write it
				if(rune_back_it < rune_forward_it)
				{
					result += stream_write(stream, Block { (void*)rune_back_it, size_t(rune_forward_it - rune_back_it)});
					//after writing the chunk we set the back iterator to the forward iterator
					rune_back_it = rune_forward_it;
				}

				//check if this is also a { thus detecting the pattern {{
				//this will print only a {
				++rune_forward_it;
				//if this is the end of the string then we encountered an error
				if(rune_forward_it == rune_end)
				{
					assert(false &&
						   "incomplete format specifier i.e. only a single '{' were found with no closing '}' in vprintf statement");
				}

				c = *rune_forward_it;
				//if we detected the {{ then write only one { and continue
				if(c == '{')
				{
					char open_brace = '{';
					result += stream_write(stream, block_from(open_brace));

					//go to the next rune and reset the back it
					++rune_forward_it;
					rune_back_it = rune_forward_it;
					//continue the upper loop
					continue;
				}

				Print_Format format;
				if(!manual_indexing)
					format.index = default_index++;
				_parse_format(rune_forward_it, rune_end, format, manual_indexing);

				//print the value
				if(format.index >= values_count)
				{
					//TODO(Moustapha): panic here
					assert(false &&
						   "index out of range in vprintf statement");
				}

				//actual call to the print_str
				result += values[format.index].print(stream, format);

				rune_back_it = rune_forward_it;
			}
			//not a special character to take it into account
			else
			{
				++rune_forward_it;
			}
		}

		//if there's some remaining text we flush it and exit
		if(rune_back_it < rune_forward_it)
		{
			result += stream_write(stream, Block { (void*)rune_back_it, size_t(rune_forward_it - rune_back_it)});
		}

		return result;
	}

	/**
	 * @brief      Writes a formatted string
	 *
	 * @param[in]  format  The format to write
	 * @param      args    The arguments to use in the format
	 *
	 * @return     A String containing the formatted string (uses tmp allocator)
	 */
	template<typename ... TArgs>
	inline static Str
	str_tmpf(const char* format, TArgs&& ... args)
	{
		Stream stream = stream_tmp();
		vprintf(stream, format, std::forward<TArgs>(args)...);
		return str_from_c(stream_str(stream), memory::tmp());
	}

	/**
	 * @brief      Writes into the given string a formatted string
	 *
	 * @param[in]  str     The string to write into
	 * @param[in]  format  The format to write
	 * @param      args    The arguments to use in the format
	 *
	 * @return     The resized and written into version of the given string
	 */
	template<typename ... TArgs>
	inline static Str
	strf(Str str, const char* format, TArgs&& ... args)
	{
		Stream stream = stream_tmp();
		vprintf(stream, format, std::forward<TArgs>(args)...);
		str_push(str, stream_str(stream));
		return str;
	}

	/**
	 * @brief      Creates a new string given the allocator with the formatted string written into it
	 *
	 * @param[in]  allocator  The allocator
	 * @param[in]  format     The format to write
	 * @param      args       The arguments to use in the format
	 *
	 * @return     The newly created string
	 */
	template<typename ... TArgs>
	inline static Str
	strf(Allocator allocator, const char* format, TArgs&& ... args)
	{
		return strf(str_with_allocator(allocator), format, std::forward<TArgs>(args)...);
	}

	/**
	 * @brief      Creates a new string with the top allocator
	 *
	 * @param[in]  format  The format to write
	 * @param      args    The arguments to use in the format
	 *
	 * @return     The newly created string
	 */
	template<typename ... TArgs>
	inline static Str
	strf(const char* format, TArgs&& ... args)
	{
		return strf(str_new(), format, std::forward<TArgs>(args)...);
	}

	template<typename ... TArgs>
	inline static size_t
	printfmt(const char* format, TArgs&& ... args)
	{
		return vprintf(stream_stdout(), format, std::forward<TArgs>(args)...);
	}

	template<typename ... TArgs>
	inline static size_t
	printfmt_err(const char* format, TArgs&& ... args)
	{
		return vprintf(stream_stderr(), format, std::forward<TArgs>(args)...);
	}


	//reader
	inline static bool
	_is_whitespace(char c)
	{
		return (c == ' '  ||
				c == '\f' ||
				c == '\n' ||
				c == '\r' ||
				c == '\t' ||
				c == '\v');
	}

	inline static void
	_guarantee_text_chunk(Reader reader, size_t REQUEST_SIZE)
	{
		size_t requested_size = 0;
		size_t last_size = size_t(-1);

		//skip all the spaces at the start of the string
		while (true)
		{
			Block bytes = reader_peek(reader, requested_size);

			if(bytes.size == last_size)
				break;

			size_t whitespace_count = 0;
			for(size_t i = 0; i < bytes.size; ++i)
			{
				char byte = ((char*)bytes.ptr)[i];
				if(_is_whitespace(byte))
					++whitespace_count;
				else
					break;
			}
			reader_skip(reader, whitespace_count);
			if (whitespace_count < bytes.size)
				break;

			requested_size += REQUEST_SIZE;
			last_size = bytes.size;
		}

		//guarantee that we didn't chop of early
		//i.e. [12345]13412\r\n
		//so we have to check for whitespaces at the end also
		//set the requested size to 0 to get the already buffered data
		requested_size = 0;
		last_size = size_t(-1);
		while (true)
		{
			bool found_whitespace = false;

			Block bytes = reader_peek(reader, requested_size);

			if(bytes.size == last_size)
				break;

			last_size = bytes.size;
			char* ptr = (char*)bytes.ptr + bytes.size;
			while(ptr-- != bytes.ptr)
			{
				if(_is_whitespace(*ptr))
				{
					found_whitespace = true;
					break;
				}
			}

			if (found_whitespace)
				break;

			requested_size += REQUEST_SIZE;
		}
	}

	inline static size_t
	_read_uint64(Reader reader, uint64_t& value, int base)
	{
		_guarantee_text_chunk(reader, 40);
		Block peeked_content = reader_peek(reader, 0);

		if(!peeked_content)
			return 0;

		char* begin = (char*)peeked_content.ptr;
		char* end = (char*)peeked_content.ptr + peeked_content.size;
		//we cannot parse negative numbers
		if(*begin == '-')
			return 0;

		uint64_t tmp_value = ::strtoull(begin, &end, base);

		if(errno == ERANGE)
			return 0;

		value = tmp_value;

		return end - begin;
	}

	inline static size_t
	_read_int64(Reader reader, int64_t& value, int base)
	{
		_guarantee_text_chunk(reader, 40);
		Block peeked_content = reader_peek(reader, 0);

		char* begin = (char*)peeked_content.ptr;
		char* end = (char*)peeked_content.ptr + peeked_content.size;
		int64_t tmp_value = ::strtoll(begin, &end, base);

		if(errno == ERANGE)
			return 0;

		value = tmp_value;

		return end - begin;
	}

	inline static size_t
	_read_float(Reader reader, float& value)
	{
		_guarantee_text_chunk(reader, 40);
		Block peeked_content = reader_peek(reader, 0);

		char* begin = (char*)peeked_content.ptr;
		char* end = (char*)peeked_content.ptr + peeked_content.size;
		float tmp_value = ::strtof(begin, &end);

		if(errno == ERANGE)
			return 0;

		value = tmp_value;

		return end - begin;
	}

	inline static size_t
	_read_double(Reader reader, double& value)
	{
		_guarantee_text_chunk(reader, 40);
		auto peeked_content = reader_peek(reader, 0);

		char* begin = (char*)peeked_content.ptr;
		char* end = (char*)peeked_content.ptr + peeked_content.size;
		double tmp_value = ::strtod(begin, &end);

		if(errno == ERANGE)
			return 0;

		value = tmp_value;

		return end - begin;
	}

	//signed functions
	#define READ_STR_SIGNED(TYPE) \
	inline static size_t \
	read_str(Reader reader, TYPE& value) \
	{ \
		int64_t tmp_value = 0; \
		size_t parsed_size = _read_int64(reader, tmp_value, 10); \
		if(parsed_size == 0) \
			return 0;\
		if (tmp_value < std::numeric_limits<TYPE>::lowest() || \
			tmp_value > std::numeric_limits<TYPE>::max()) \
		{ \
			assert(false && "signed integer overflow"); \
			return 0; \
		} \
		value = static_cast<TYPE>(tmp_value); \
		return reader_skip(reader, parsed_size); \
	}

	READ_STR_SIGNED(int8_t)
	READ_STR_SIGNED(int16_t)
	READ_STR_SIGNED(int32_t)
	READ_STR_SIGNED(int64_t)

	#undef READ_STR_SIGNED

	inline static size_t
	read_str(Reader reader, char& value)
	{
		return reader_read(reader, block_from(value));
	}

	//unsigned functions
	#define READ_STR_UNSIGNED(TYPE) \
	inline static size_t \
	read_str(Reader reader, TYPE& value) \
	{ \
		uint64_t tmp_value = 0; \
		size_t parsed_size = _read_uint64(reader, tmp_value, 10); \
		if(parsed_size == 0) \
			return 0;\
		if (tmp_value > std::numeric_limits<TYPE>::max()) \
		{ \
			assert(false && "unsigned integer overflow"); \
			return 0; \
		} \
		value = static_cast<TYPE>(tmp_value); \
		return reader_skip(reader, parsed_size); \
	}

	READ_STR_UNSIGNED(uint8_t)
	READ_STR_UNSIGNED(uint16_t)
	READ_STR_UNSIGNED(uint32_t)
	READ_STR_UNSIGNED(uint64_t)

	#undef READ_STR_UNSIGNED

	inline static size_t
	read_str(Reader reader, void*& value)
	{
		uint64_t tmp_value = 0;
		size_t parsed_size = _read_uint64(reader, tmp_value, 16);
		if(parsed_size == 0)
			return 0;
		if(tmp_value > std::numeric_limits<size_t>::max())
		{
			assert(false && "pointer address overflow");
			return 0;
		}
		value = reinterpret_cast<void*>(tmp_value);
		return reader_skip(reader, parsed_size);
	}

	inline static size_t
	read_str(Reader reader, float& value)
	{
		float tmp_value = 0;
		size_t parsed_size = _read_float(reader, tmp_value);
		if(parsed_size == 0)
		{
			assert(false && "float overflow");
			return 0;
		}
		value = tmp_value;
		return reader_skip(reader, parsed_size);
	}

	inline static size_t
	read_str(Reader reader, double& value)
	{
		double tmp_value = 0;
		size_t parsed_size = _read_double(reader, tmp_value);
		if(parsed_size == 0)
		{
			assert(false && "double overflow");
			return 0;
		}
		value = tmp_value;
		return reader_skip(reader, parsed_size);
	}

	inline static size_t
	read_str(Reader reader, Str& value)
	{
		_guarantee_text_chunk(reader, 1024);
		auto bytes = reader_peek(reader, 0);
		size_t non_whitespace_count = 0;
		for(size_t i = 0; i < bytes.size; ++i)
		{
			char byte = ((char*)bytes.ptr)[i];
			if(_is_whitespace(byte))
				break;
			++non_whitespace_count;
		}

		if(non_whitespace_count == 0)
			return 0;

		str_clear(value);
		str_block_push(value, Block { bytes.ptr, non_whitespace_count });
		return reader_skip(reader, non_whitespace_count);
	}

	inline static size_t
	readln(Reader reader, Str& value)
	{
		size_t newline_offset = size_t(-1);
		size_t last_size = size_t(-1);
		size_t request_size = 0;
		while(true)
		{
			auto bytes = reader_peek(reader, request_size);

			bool found_newline = false;
			for(size_t i = 0; i < bytes.size; ++i)
			{
				char byte = ((char*)bytes.ptr)[i];
				if(byte == '\n')
				{
					found_newline = true;
					newline_offset = i;
					break;
				}
			}

			if(found_newline)
				break;
			else if(last_size == bytes.size)
				break;

			request_size += 1024;
			last_size = bytes.size;
		}

		auto bytes = reader_peek(reader, 0);
		str_clear(value);
		if(newline_offset != size_t(-1))
		{
			size_t additional_skip = 1;
			if(newline_offset > 0 &&
			   ((char*)bytes.ptr)[newline_offset - 1] == '\r')
			{
				//because of the \r\n on window
				--newline_offset;
				++additional_skip;
			}

			str_block_push(value, Block { bytes.ptr, newline_offset });
			return reader_skip(reader, newline_offset + additional_skip) - additional_skip;
		}
		else
		{
			str_block_push(value, bytes);
			return reader_skip(reader, bytes.size);
		}
	}

	inline static size_t
	readln(Str& value)
	{
		return readln(reader_stdin(), value);
	}

	inline static void
	_variadic_read_string_helper(Reader, size_t&)
	{
		return;
	}

	template<typename TFirst, typename ... TArgs>
	inline static void
	_variadic_read_string_helper(Reader reader,
								 size_t& size,
								 TFirst&& first_arg,
								 TArgs&& ... args)
	{
		size += read_str(reader, std::forward<TFirst>(first_arg)) != 0;
		_variadic_read_string_helper(reader, size, std::forward<TArgs>(args)...);
	}

	template<typename ... TArgs>
	inline static size_t
	vreads(Reader reader, TArgs&& ... args)
	{
		size_t result = 0;
		_variadic_read_string_helper(reader, result, std::forward<TArgs>(args)...);
		return result;
	}

	template<typename ... TArgs>
	inline static size_t
	reads(const Str& str, TArgs&& ... args)
	{
		Reader reader = _reader_tmp();
		reader = reader_wrap_str(reader, str);
		size_t result = 0;
		_variadic_read_string_helper(reader, result, std::forward<TArgs>(args)...);
		return result;
	}

	template<typename ... TArgs>
	inline static size_t
	reads(const char* str, TArgs&& ... args)
	{
		return reads(str_lit(str), std::forward<TArgs>(args)...);
	}

	template<typename ... TArgs>
	inline static size_t
	read(TArgs&& ... args)
	{
		return vreads(reader_stdin(), std::forward<TArgs>(args)...);
	}


	template<typename T>
	inline static size_t
	read_bin(Stream stream, T& value)
	{
		return stream_read(stream, block_from(value));
	}

	template<typename T>
	inline static size_t
	read_bin(Reader reader, T& value)
	{
		return reader_read(reader, block_from(value));
	}

	inline static size_t
	read_bin(Stream stream, Block value)
	{
		return stream_read(stream, value);
	}

	inline static size_t
	read_bin(Reader reader, Block value)
	{
		return reader_read(reader, value);
	}

	inline static void
	_variadic_read_binary_helper(Stream, size_t&)
	{
		return;
	}

	template<typename TFirst, typename ... TArgs>
	inline static void
	_variadic_read_binary_helper(Stream stream,
								 size_t& size,
								 TFirst&& first_arg,
								 TArgs&& ... args)
	{
		size += read_bin(stream, std::forward<TFirst>(first_arg));
		_variadic_read_binary_helper(stream, size, std::forward<TArgs>(args)...);
	}

	inline static void
	_variadic_read_binary_helper(Reader, size_t&)
	{
		return;
	}

	template<typename TFirst, typename ... TArgs>
	inline static void
	_variadic_read_binary_helper(Reader reader,
								 size_t& size,
								 TFirst&& first_arg,
								 TArgs&& ... args)
	{
		size += read_bin(reader, std::forward<TFirst>(first_arg));
		_variadic_read_binary_helper(reader, size, std::forward<TArgs>(args)...);
	}

	template<typename ... TArgs>
	inline static size_t
	vreadb(Stream stream, TArgs&& ... args)
	{
		size_t result = 0;
		_variadic_read_binary_helper(stream, result, std::forward<TArgs>(args)...);
		return result;
	}

	template<typename ... TArgs>
	inline static size_t
	vreadb(Reader reader, TArgs&& ... args)
	{
		size_t result = 0;
		_variadic_read_binary_helper(reader, result, std::forward<TArgs>(args)...);
		return result;
	}
}
