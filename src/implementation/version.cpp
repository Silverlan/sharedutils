// SPDX-FileCopyrightText: (c) 2019 Silverlan <opensource@pragma-engine.com>
// SPDX-License-Identifier: MIT

module;

module pragma.util;

import :version;

std::ostream &operator<<(std::ostream &out, const pragma::util::Version &version)
{
	out << version.ToString();
	return out;
}

pragma::util::Version::Version(unsigned int _major, unsigned int _minor, unsigned int _revision) : major(_major), minor(_minor), revision(_revision) {}
pragma::util::Version::Version(unsigned int _major, unsigned int _minor) : Version(_major, _minor, 0) {}
pragma::util::Version::Version(const std::string &version) : Version {FromString(version)} {}
pragma::util::Version::Version() : Version(0, 0) {}
void pragma::util::Version::Reset()
{
	major = 0;
	minor = 0;
	revision = 0;
}
std::string pragma::util::Version::ToString() const
{
	auto r = std::to_string(major) + std::string(".") + std::to_string(minor);
	if(revision > 0)
		r += std::string(".") + std::to_string(revision);
	return r;
}
pragma::util::Version pragma::util::Version::FromString(const std::string &version)
{
	std::vector<std::string> components;
	components.reserve(3);
	string::explode(version, ".", components);
	Version r {};
	if(!components.empty()) {
		r.major = to_int(components.front());
		if(components.size() > 1) {
			r.minor = to_int(components[1]);
			if(components.size() > 2)
				r.revision = to_int(components[2]);
		}
	}
	return r;
}
bool pragma::util::Version::operator==(const Version &other) const { return (minor == other.minor && major == other.major && revision == other.revision) ? true : false; }
bool pragma::util::Version::operator!=(const Version &other) const { return (*this == other) ? false : true; }
bool pragma::util::Version::operator>(const Version &other) const { return (major > other.major || (major == other.major && (minor > other.minor || (minor == other.minor && revision > other.revision)))) ? true : false; }
bool pragma::util::Version::operator<(const Version &other) const { return (major < other.major || (major == other.major && (minor < other.minor || (minor == other.minor && revision < other.revision)))) ? true : false; }
bool pragma::util::Version::operator>=(const Version &other) const { return (*this == other || *this > other) ? true : false; }
bool pragma::util::Version::operator<=(const Version &other) const { return (*this == other || *this < other) ? true : false; }
