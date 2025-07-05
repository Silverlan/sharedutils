// SPDX-FileCopyrightText: (c) 2019 Silverlan <opensource@pragma-engine.com>
// SPDX-License-Identifier: MIT

#ifndef __UTIL_CASE_INSENSITIVE_HASH_HPP__
#define __UTIL_CASE_INSENSITIVE_HASH_HPP__

#include "utildefinitions.h"
#include <unordered_map>
#include <string>

namespace util {
	struct DLLSHUTIL CaseInsensitiveStringComparator {
		bool operator()(const std::string &lhs, const std::string &rhs) const;
	};
	struct DLLSHUTIL CaseInsensitiveHashingFunc {
		unsigned long operator()(const std::string &key) const;
	};
	template<typename T>
	using CIMap = std::unordered_map<std::string, T, CaseInsensitiveHashingFunc, CaseInsensitiveStringComparator>;
	// Does not work correctly? Find out why
	//using KeyValueMap = std::unordered_map<std::string,std::string,util::CaseInsensitiveHashingFunc,util::CaseInsensitiveStringComparator>;
};

#endif
