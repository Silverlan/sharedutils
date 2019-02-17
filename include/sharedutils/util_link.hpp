/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef __UTIL_LINK_HPP__
#define __UTIL_LINK_HPP__

#include "utildefinitions.h"
#include <string>

namespace util
{
	DLLSHUTIL bool create_link(const std::string &srcPath,const std::string &lnkPath);
	DLLSHUTIL bool resolve_link(const std::string &lnkPath,std::string &outResolvedPath);
	DLLSHUTIL bool link_exists(const std::string &lnkPath);
};

#endif
