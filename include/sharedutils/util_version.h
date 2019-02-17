/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef __UTIL_VERSION_H__
#define __UTIL_VERSION_H__

#include "utildefinitions.h"
#include <string>

#ifdef __linux__
	#undef major
	#undef minor
#endif

namespace util
{
#ifdef _WIN32
	#pragma pack(push,1)
#endif
	struct DLLSHUTIL Version
	{
		Version(unsigned int _major,unsigned int _minor,unsigned int _revision=0);
		Version();
		unsigned int major;
		unsigned int minor;
		unsigned int revision;
		void Reset();
		std::string ToString() const;
		static Version FromString(const std::string &version);
		bool operator==(const Version &other) const;
		bool operator!=(const Version &other) const;
		bool operator>(const Version &other) const;
		bool operator<(const Version &other) const;
		bool operator>=(const Version &other) const;
		bool operator<=(const Version &other) const;
	}
#ifdef __linux__
	__attribute__((packed))
#endif
	;
#ifdef _WIN32
	#pragma pack(pop)
#endif
}

#endif