/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef __UTIL_HETEROGENOUS_LOOKUP_HPP__
#define __UTIL_HETEROGENOUS_LOOKUP_HPP__

#include <utility>
#include <string>
#include <string_view>

namespace util {
	struct hl_string_hash {
		using is_transparent = void;
		[[nodiscard]] size_t operator()(const char *txt) const { return std::hash<std::string_view> {}(txt); }
		[[nodiscard]] size_t operator()(std::string_view txt) const { return std::hash<std::string_view> {}(txt); }
		[[nodiscard]] size_t operator()(const std::string &txt) const { return std::hash<std::string> {}(txt); }
	};
};

#endif
