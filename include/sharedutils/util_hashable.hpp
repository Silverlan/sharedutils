/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef __UTIL_HASHABLE_HPP__
#define __UTIL_HASHABLE_HPP__

#include "utildefinitions.h"
#include <cinttypes>
#include <unordered_map>

// Source: https://stackoverflow.com/q/21900707/2482983
class DLLSHUTIL Hashable
{
public:
	virtual ~Hashable() {}
	virtual std::size_t Hash() const=0;
};
#define DEFINE_STD_HASH_SPECIALIZATION(hashable)                              \
	namespace std {                                                           \
		template<>                                                            \
			struct hash<hashable> {                                           \
				std::size_t operator()(const hashable& object) const {        \
				return object.Hash();                                         \
				}                                                                 \
			};                                                                    \
	}

#endif
