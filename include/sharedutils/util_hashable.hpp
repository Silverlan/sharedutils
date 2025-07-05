// SPDX-FileCopyrightText: (c) 2019 Silverlan <opensource@pragma-engine.com>
// SPDX-License-Identifier: MIT

#ifndef __UTIL_HASHABLE_HPP__
#define __UTIL_HASHABLE_HPP__

#include "utildefinitions.h"
#include <cinttypes>
#include <unordered_map>

// Source: https://stackoverflow.com/q/21900707/2482983
class DLLSHUTIL Hashable {
  public:
	virtual ~Hashable() {}
	virtual std::size_t Hash() const = 0;
};
#define DEFINE_STD_HASH_SPECIALIZATION(hashable)                                                                                                                                                                                                                                                 \
	namespace std {                                                                                                                                                                                                                                                                              \
		template<>                                                                                                                                                                                                                                                                               \
		struct hash<hashable> {                                                                                                                                                                                                                                                                  \
			std::size_t operator()(const hashable &object) const                                                                                                                                                                                                                                 \
			{                                                                                                                                                                                                                                                                                    \
				return object.Hash();                                                                                                                                                                                                                                                            \
			}                                                                                                                                                                                                                                                                                    \
		};                                                                                                                                                                                                                                                                                       \
	}

#endif
