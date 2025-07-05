// SPDX-FileCopyrightText: (c) 2019 Silverlan <opensource@pragma-engine.com>
// SPDX-License-Identifier: MIT

#ifndef __UTIL_LINK_HPP__
#define __UTIL_LINK_HPP__

#include "utildefinitions.h"
#include <string>

namespace util {
	DLLSHUTIL bool create_link(const std::string &srcPath, const std::string &lnkPath);
	DLLSHUTIL bool resolve_link(const std::string &lnkPath, std::string &outResolvedPath);
	DLLSHUTIL bool link_exists(const std::string &lnkPath);
};

#endif
