// SPDX-FileCopyrightText: (c) 2019 Silverlan <opensource@pragma-engine.com>
// SPDX-License-Identifier: MIT

#ifndef __UTIL_VERSION_H__
#define __UTIL_VERSION_H__

#include "utildefinitions.h"
#include <iostream>
#include <string>

#ifdef __linux__
#undef major
#undef minor
#endif

namespace util {
#ifdef _WIN32
#pragma pack(push, 1)
#endif
	struct DLLSHUTIL Version {
		Version(unsigned int _major, unsigned int _minor, unsigned int _revision);
		Version(unsigned int _major, unsigned int _minor);
		Version(const std::string &version);
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

DLLSHUTIL std::ostream &operator<<(std::ostream &out, const util::Version &version);

#endif
