/* --------------------------------- ABOUT -------------------------------------

Original Author: Adam Yaxley
Website: https://github.com/adamyaxley
License: See end of file

Obfuscate
Guaranteed compile-time string literal obfuscation library for C++14

Usage:
Pass string literals into the AY_OBFUSCATE macro to obfuscate them at compile
time. AY_OBFUSCATE returns a reference to an ay::obfuscated_data object with the
following traits:
	- Guaranteed obfuscation of string
	The passed string is encrypted with a simple XOR cipher at compile-time to
	prevent it being viewable in the binary image
	- Global lifetime
	The actual instantiation of the ay::obfuscated_data takes place inside a
	lambda as a function level static
	- Implicitly convertible to a char*
	This means that you can pass it directly into functions that would normally
	take a char* or a const char*

Example:
const char* obfuscated_string = AY_OBFUSCATE("Hello World");
std::cout << obfuscated_string << std::endl;

----------------------------------------------------------------------------- */

#pragma once
#if __cplusplus >= 202002L
	#define AY_CONSTEVAL consteval
#else
	#define AY_CONSTEVAL constexpr
#endif

// Workaround for __LINE__ not being constexpr when /ZI (Edit and Continue) is enabled in Visual Studio
// See: https://developercommunity.visualstudio.com/t/-line-cannot-be-used-as-an-argument-for-constexpr/195665
#ifdef _MSC_VER
	#define AY_CAT(X,Y) AY_CAT2(X,Y)
	#define AY_CAT2(X,Y) X##Y
	#define AY_LINE int(AY_CAT(__LINE__,U))
#else
	#define AY_LINE __LINE__
#endif

#ifndef AY_OBFUSCATE_DEFAULT_KEY
	// The default 64 bit key to obfuscate strings with.
	// This can be user specified by defining AY_OBFUSCATE_DEFAULT_KEY before 
	// including obfuscate.h
	#define AY_OBFUSCATE_DEFAULT_KEY ay::generate_key(AY_LINE)
#endif

namespace ay
{
	using size_type = unsigned long long;
	using key_type = unsigned long long;

	// libstdc++ has std::remove_cvref_t<T> since C++20, but because not every user will be 
	// able or willing to link to the STL, we prefer to do this functionality ourselves here.
	template <typename T>
	struct remove_const_ref {
		using type = T;
	};

	template <typename T>
	struct remove_const_ref<T&> {
		using type = T;
	};

	template <typename T>
	struct remove_const_ref<const T> {
		using type = T;
	};

	template <typename T>
	struct remove_const_ref<const T&> {
		using type = T;
	};

	template <typename T>
	using char_type = typename remove_const_ref<T>::type;

	// Generate a pseudo-random key that spans all 8 bytes
	AY_CONSTEVAL key_type generate_key(key_type seed)
	{
		// Use the MurmurHash3 64-bit finalizer to hash our seed
		key_type key = seed;
		key ^= (key >> 33);
		key *= 0xff51afd7ed558ccd;
		key ^= (key >> 33);
		key *= 0xc4ceb9fe1a85ec53;
		key ^= (key >> 33);

		// Make sure that a bit in each byte is set
		key |= 0x0101010101010101ull;

		return key;
	}

	// Obfuscates or deobfuscates data with key
	template <typename CHAR_TYPE>
	constexpr void cipher(CHAR_TYPE* data, size_type size, key_type key)
	{
		// Obfuscate with a simple XOR cipher based on key
		for (size_type i = 0; i < size; i++)
		{
			data[i] ^= CHAR_TYPE((key >> ((i % 8) * 8)) & 0xFF);
		}
	}

	// Obfuscates a string at compile time
	template <size_type N, key_type KEY, typename CHAR_TYPE = char>
	class obfuscator
	{
	public:
		// Obfuscates the string 'data' on construction
		AY_CONSTEVAL obfuscator(const CHAR_TYPE* data)
		{
			// Copy data
			for (size_type i = 0; i < N; i++)
			{
				m_data[i] = data[i];
			}

			// On construction each of the characters in the string is
			// obfuscated with an XOR cipher based on key
			cipher(m_data, N, KEY);
		}

		constexpr const CHAR_TYPE* data() const
		{
			return &m_data[0];
		}

		AY_CONSTEVAL size_type size() const
		{
			return N;
		}

		AY_CONSTEVAL key_type key() const
		{
			return KEY;
		}

	private:

		CHAR_TYPE m_data[N]{};
	};

	// Handles decryption and re-encryption of an encrypted string at runtime
	template <size_type N, key_type KEY, typename CHAR_TYPE = char>
	class obfuscated_data
	{
	public:
		obfuscated_data(const obfuscator<N, KEY, CHAR_TYPE>& obfuscator)
		{
			// Copy obfuscated data
			for (size_type i = 0; i < N; i++)
			{
				m_data[i] = obfuscator.data()[i];
			}
		}

		~obfuscated_data()
		{
			// Zero m_data to remove it from memory
			for (size_type i = 0; i < N; i++)
			{
				m_data[i] = 0;
			}
		}

		// Returns a pointer to the plain text string, decrypting it if
		// necessary
		operator CHAR_TYPE* ()
		{
			decrypt();
			return m_data;
		}

		// Manually decrypt the string
		void decrypt()
		{
			if (m_encrypted)
			{
				cipher(m_data, N, KEY);
				m_encrypted = false;
			}
		}

		// Manually re-encrypt the string
		void encrypt()
		{
			if (!m_encrypted)
			{
				cipher(m_data, N, KEY);
				m_encrypted = true;
			}
		}

		// Returns true if this string is currently encrypted, false otherwise.
		bool is_encrypted() const
		{
			return m_encrypted;
		}

	private:

		// Local storage for the string. Call is_encrypted() to check whether or
		// not the string is currently obfuscated.
		CHAR_TYPE m_data[N];

		// Whether data is currently encrypted
		bool m_encrypted{ true };
	};

	// This function exists purely to extract the number of elements 'N' in the
	// array 'data'
	template <size_type N, key_type KEY = AY_OBFUSCATE_DEFAULT_KEY, typename CHAR_TYPE = char>
	AY_CONSTEVAL auto make_obfuscator(const CHAR_TYPE(&data)[N])
	{
		return obfuscator<N, KEY, CHAR_TYPE>(data);
	}
}

// Obfuscates the string 'data' at compile-time and returns a reference to a
// ay::obfuscated_data object with global lifetime that has functions for
// decrypting the string and is also implicitly convertable to a char*
#define AY_OBFUSCATE(data) AY_OBFUSCATE_KEY(data, AY_OBFUSCATE_DEFAULT_KEY)

// Obfuscates the string 'data' with 'key' at compile-time and returns a
// reference to a ay::obfuscated_data object with global lifetime that has
// functions for decrypting the string and is also implicitly convertable to a
// char*
#define AY_OBFUSCATE_KEY(data, key) \
	[]() -> ay::obfuscated_data<sizeof(data)/sizeof(data[0]), key, ay::char_type<decltype(*data)>>& { \
		static_assert(sizeof(decltype(key)) == sizeof(ay::key_type), "key must be a 64 bit unsigned integer"); \
		static_assert((key) >= (1ull << 56), "key must span all 8 bytes"); \
		using char_type = ay::char_type<decltype(*data)>; \
		constexpr auto n = sizeof(data)/sizeof(data[0]); \
		constexpr auto obfuscator = ay::make_obfuscator<n, key, char_type>(data); \
		thread_local auto obfuscated_data = ay::obfuscated_data<n, key, char_type>(obfuscator); \
		return obfuscated_data; \
	}()

#ifndef NO_OBFUSCATE
#define _O(x) AY_OBFUSCATE(x)
#else
#define _O(x) x
#endif

/* -------------------------------- LICENSE ------------------------------------

Public Domain (http://www.unlicense.org)

This is free and unencumbered software released into the public domain.

Anyone is free to copy, modify, publish, use, compile, sell, or distribute this
software, either in source code form or as a compiled binary, for any purpose,
commercial or non-commercial, and by any means.

In jurisdictions that recognize copyright laws, the author or authors of this
software dedicate any and all copyright interest in the software to the public
domain. We make this dedication for the benefit of the public at large and to
the detriment of our heirs and successors. We intend this dedication to be an
overt act of relinquishment in perpetuity of all present and future rights to
this software under copyright law.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS BE
LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF
CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

----------------------------------------------------------------------------- */
