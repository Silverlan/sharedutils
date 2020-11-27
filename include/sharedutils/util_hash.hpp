/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef __UTIL_HASH_HPP__
#define __UTIL_HASH_HPP__

#include "sharedutils/utildefinitions.h"
#include <limits>
#include <cstdint>
#include <type_traits>

// Source: https://stackoverflow.com/a/50978188/2482983
namespace util
{
	using Hash = uint64_t;
	template<typename T>
		T xorshift(const T& n,int i)
		{
			return n^(n>>i);
		}

	DLLSHUTIL uint32_t distribute(const uint32_t& n);
	DLLSHUTIL Hash hash(const Hash& n);

	// if c++20 rotl is not available:
	template <typename T,typename S>
	typename std::enable_if<std::is_unsigned<T>::value,T>::type
	constexpr rotl(const T n, const S i)
	{
			const T m = (std::numeric_limits<T>::digits-1);
		const T c = i&m;
		return (n<<c)|(n>>((T(0)-c)&m)); // this is usually recognized by the compiler to mean rotation, also c++20 now gives us rotl directly
	}

	template <class T>
		inline Hash hash_combine(Hash seed, const T& v)
	{
		return rotl(seed,std::numeric_limits<size_t>::digits/3) ^ distribute(std::hash<T>()(v));
	}
};

#endif
