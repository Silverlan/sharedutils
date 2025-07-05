// SPDX-FileCopyrightText: (c) 2019 Silverlan <opensource@pragma-engine.com>
// SPDX-License-Identifier: MIT

#include "sharedutils/util_case_insensitive_hash.hpp"
#include <unordered_map>
#ifdef __linux__
#include <strings.h>
#endif

unsigned long util::CaseInsensitiveHashingFunc::operator()(const std::string &key) const
{
	// djb2 hash
	size_t hash = 5381;
	auto len = key.length();
	for(auto i = 0; i < key.length(); ++i) {
		auto idx = i;
		auto c = (unsigned char)key[i];
		if(c == '\\')
			c = '/';
		if(c == '/') {
			if(idx == 0 || idx == (len - 1))
				continue;
		}
		c = std::tolower(static_cast<unsigned char>(c));

		hash = ((hash << 5) + hash) + c; /* hash * 33 + c */
	}
	return hash;
}

#ifdef _WIN32
#define STRICMP _stricmp
#else
#define STRICMP strcasecmp
#endif

bool util::CaseInsensitiveStringComparator::operator()(const std::string &lhs, const std::string &rhs) const
{
	return lhs.size() == rhs.size() && std::equal(lhs.begin(), lhs.end(), rhs.begin(), [](char a, char b) { return tolower(a) == tolower(b); });
}
