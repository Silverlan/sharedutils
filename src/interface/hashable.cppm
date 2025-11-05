// SPDX-FileCopyrightText: (c) 2019 Silverlan <opensource@pragma-engine.com>
// SPDX-License-Identifier: MIT

module;

#include "definitions.hpp"

export module pragma.util:hashable;

export import std.compat;

export namespace util {
	template<typename T>
	concept HasMemberHash = requires(const T& t) {
		{ t.Hash() } -> std::convertible_to<std::size_t>;
	};

	template<HasMemberHash T>
	struct hash_by_member {
		std::size_t operator()(const T& t) const noexcept(noexcept(t.Hash())) {
			return t.Hash();
		}
	};
};

// Usage to enable hashing for type T:
// namespace std { template<> struct hash<T> : util::hash_by_member<T> {}; }
