/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef __UTIL_CASE_INSENSITIVE_HASH_HPP__
#define __UTIL_CASE_INSENSITIVE_HASH_HPP__

#include "utildefinitions.h"
#include <unordered_map>
#include <string>

namespace util
{
	struct DLLSHUTIL CaseInsensitiveStringComparator
	{
		bool operator()(const std::string &lhs,const std::string &rhs) const;
	};
	struct DLLSHUTIL CaseInsensitiveHashingFunc
	{
		unsigned long operator()(const std::string& key) const;
	};
	// Does not work correctly? Find out why
	// using KeyValueMap = std::unordered_map<std::string,std::string,util::CaseInsensitiveHashingFunc,util::CaseInsensitiveStringComparator>;
};

#endif
