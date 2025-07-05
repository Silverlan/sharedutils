// SPDX-FileCopyrightText: (c) 2021 Silverlan <opensource@pragma-engine.com>
// SPDX-License-Identifier: MIT

#ifndef __UTIL_STRING_LITERAL_HPP__
#define __UTIL_STRING_LITERAL_HPP__

#include <algorithm>

namespace util {
	template<size_t N>
	struct StringLiteral {
		constexpr StringLiteral(const char (&str)[N]) { std::copy_n(str, N, value); }

		char value[N];
	};
};

#endif
