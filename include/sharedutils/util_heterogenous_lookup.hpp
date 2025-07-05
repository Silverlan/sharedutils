// SPDX-FileCopyrightText: (c) 2025 Silverlan <opensource@pragma-engine.com>
// SPDX-License-Identifier: MIT

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
