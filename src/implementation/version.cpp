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

pragma::util::Version::Version(const std::string &version) : Version {FromString(version)} {}
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
