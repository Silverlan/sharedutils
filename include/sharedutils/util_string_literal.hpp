/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

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
