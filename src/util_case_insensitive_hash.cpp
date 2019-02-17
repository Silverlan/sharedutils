/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "sharedutils/util_case_insensitive_hash.hpp"
#include <unordered_map>
#ifdef __linux__
#include <strings.h>
#endif

unsigned long util::CaseInsensitiveHashingFunc::operator()(const std::string& key) const
{
	auto hash = 0ul;
	for(auto v : key)
		hash += (71 *hash +::tolower(v)) %5;
	return hash;
}

#ifdef _WIN32
#define STRICMP _stricmp
#else
#define STRICMP strcasecmp
#endif

bool util::CaseInsensitiveStringComparator::operator()(const std::string &lhs,const std::string &rhs) const
{
	return STRICMP(lhs.c_str(),rhs.c_str()) < 0;
}
