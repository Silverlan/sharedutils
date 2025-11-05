// SPDX-FileCopyrightText: (c) 2020 Silverlan <opensource@pragma-engine.com>
// SPDX-License-Identifier: MIT

module;

#include "definitions.hpp"

export module pragma.util:hash;

export import std.compat;

// Source: https://stackoverflow.com/a/50978188/2482983
export namespace util {
	using Hash = uint64_t;
	template<typename T>
	T xorshift(const T &n, int i)
	{
		return n ^ (n >> i);
	}

	DLLSHUTIL uint32_t distribute(const uint32_t &n);
	DLLSHUTIL Hash hash(const Hash &n);

	// if c++20 rotl is not available:
	template<typename T, typename S>
	typename std::enable_if<std::is_unsigned<T>::value, T>::type constexpr rotl(const T n, const S i)
	{
		const T m = (std::numeric_limits<T>::digits - 1);
		const T c = i & m;
		return (n << c) | (n >> ((T(0) - c) & m)); // this is usually recognized by the compiler to mean rotation, also c++20 now gives us rotl directly
	}

	template<class T>
	inline Hash hash_combine(Hash seed, const T &v)
	{
		return rotl(seed, std::numeric_limits<size_t>::digits / 3) ^ distribute(std::hash<T>()(v));
	}

	struct pair_hash {
		template<class T1, class T2>
		std::size_t operator()(const std::pair<T1, T2> &pair) const
		{
			util::Hash hash = 0;
			hash = util::hash_combine<size_t>(hash, std::hash<T1> {}(pair.first));
			hash = util::hash_combine<size_t>(hash, std::hash<T2> {}(pair.second));
			return hash;
		}
	};

};
