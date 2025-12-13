// SPDX-FileCopyrightText: (c) 2021 Silverlan <opensource@pragma-engine.com>
// SPDX-License-Identifier: MIT

module;

export module pragma.util:string_literal;

export import std.compat;

export namespace pragma::util {
	template<size_t N>
	struct StringLiteral {
		constexpr StringLiteral(const char (&str)[N]) { std::copy_n(str, N, value); }

		char value[N];
	};
};
