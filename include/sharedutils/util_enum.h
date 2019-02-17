/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef __UTIL_ENUM_H__
#define __UTIL_ENUM_H__

#include "utildefinitions.h"

namespace util
{
	// Required as a hash when using std::unordered_map with an enum class as key
	struct DLLSHUTIL EnumHash
	{
		template<typename T>
			std::size_t operator()(T t) const;
	};
};

template<typename T>
	std::size_t util::EnumHash::operator()(T t) const
{
	return static_cast<std::size_t>(t);
}

#endif
