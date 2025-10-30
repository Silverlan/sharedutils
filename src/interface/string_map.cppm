// SPDX-FileCopyrightText: (c) 2021 Silverlan <opensource@pragma-engine.com>
// SPDX-License-Identifier: MIT

module;

#include "sharedutils/utildefinitions.h"

export module pragma.util:string_map;

export import std.compat;

export namespace util {
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
