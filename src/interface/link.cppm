// SPDX-FileCopyrightText: (c) 2019 Silverlan <opensource@pragma-engine.com>
// SPDX-License-Identifier: MIT

module;

#include "sharedutils/utildefinitions.h"
#include <string>

export module pragma.util:link;

export namespace util {
	DLLSHUTIL bool create_link(const std::string &srcPath, const std::string &lnkPath);
	DLLSHUTIL bool resolve_link(const std::string &lnkPath, std::string &outResolvedPath);
	DLLSHUTIL bool link_exists(const std::string &lnkPath);
};
