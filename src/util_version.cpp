/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "sharedutils/util_version.h"
#include "sharedutils/util.h"
#include "sharedutils/util_string.h"

util::Version::Version(unsigned int _major,unsigned int _minor,unsigned int _revision)
	: major(_major),minor(_minor),revision(_revision)
{}
util::Version::Version()
	: Version(0,0)
{}
void util::Version::Reset() {major = 0; minor = 0; revision = 0;}
std::string util::Version::ToString() const
{
	auto r = std::to_string(major) +std::string(".") +std::to_string(minor);
	if(revision > 0)
		r += std::string(".") +std::to_string(revision);
	return r;
}
util::Version util::Version::FromString(const std::string &version)
{
	std::vector<std::string> components;
	components.reserve(3);
	ustring::explode(version,".",components);
	Version r {};
	if(!components.empty())
	{
		r.major = util::to_int(components.front());
		if(components.size() > 1)
		{
			r.minor = util::to_int(components[1]);
			if(components.size() > 2)
				r.revision = util::to_int(components[2]);
		}
	}
	return r;
}
bool util::Version::operator==(const Version &other) const {return (minor == other.minor && major == other.major && revision == other.revision) ? true : false;}
bool util::Version::operator!=(const Version &other) const {return (*this == other) ? false : true;}
bool util::Version::operator>(const Version &other) const
{
	return (major > other.major || (major == other.major && (minor > other.minor || (minor == other.minor && revision > other.revision)))) ? true : false;
}
bool util::Version::operator<(const Version &other) const
{
	return (major < other.major || (major == other.major && (minor < other.minor || (minor == other.minor && revision < other.revision)))) ? true : false;
}
bool util::Version::operator>=(const Version &other) const {return (*this == other || *this > other) ? true : false;}
bool util::Version::operator<=(const Version &other) const {return (*this == other || *this < other) ? true : false;}