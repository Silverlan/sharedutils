/* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at http://mozilla.org/MPL/2.0/.
*
* Copyright (c) 2021 Silverlan
*/

#ifndef __UTIL_STRING_HASH_HPP__
#define __UTIL_STRING_HASH_HPP__

#include "utildefinitions.h"
#include <unordered_map>

namespace util {
	// See https://en.cppreference.com/w/cpp/container/unordered_map/find
	struct DLLSHUTIL string_hash {
		using hash_type = std::hash<std::string_view>;
		using is_transparent = void;

		size_t operator()(const char *str) const { return hash_type {}(str); }
		size_t operator()(std::string_view str) const { return hash_type {}(str); }
		size_t operator()(std::string const &str) const { return hash_type {}(str); }
	};
	template<typename T>
	using StringMap = std::unordered_map<std::string, T, string_hash, std::equal_to<>>;
};

#endif
