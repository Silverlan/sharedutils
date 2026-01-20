// SPDX-FileCopyrightText: (c) 2019 Silverlan <opensource@pragma-engine.com>
// SPDX-License-Identifier: MIT

module;

#ifdef __linux__
#undef major
#undef minor
#endif

export module pragma.util:version;

export import std.compat;

export {
	namespace pragma::util {
#ifdef _WIN32
#pragma pack(push, 1)
#endif
		struct DLLSHUTIL Version {
			constexpr Version(unsigned int _major, unsigned int _minor, unsigned int _revision) : major(_major), minor(_minor), revision(_revision) {}
			constexpr Version(unsigned int _major, unsigned int _minor) : Version(_major, _minor, 0) {}
			Version(const std::string &version);
			constexpr Version() : Version(0, 0) {}
			unsigned int major;
			unsigned int minor;
			unsigned int revision;
			constexpr void Reset()
			{
				major = 0;
				minor = 0;
				revision = 0;
			}
			std::string ToString() const;
			static Version FromString(const std::string &version);
			constexpr bool operator==(const Version &other) const { return (minor == other.minor && major == other.major && revision == other.revision) ? true : false; }
			constexpr bool operator!=(const Version &other) const { return (*this == other) ? false : true; }
			constexpr bool operator>(const Version &other) const { return (major > other.major || (major == other.major && (minor > other.minor || (minor == other.minor && revision > other.revision)))) ? true : false; }
			constexpr bool operator<(const Version &other) const { return (major < other.major || (major == other.major && (minor < other.minor || (minor == other.minor && revision < other.revision)))) ? true : false; }
			constexpr bool operator>=(const Version &other) const { return (*this == other || *this > other) ? true : false; }
			constexpr bool operator<=(const Version &other) const { return (*this == other || *this < other) ? true : false; }
		}
#ifdef __linux__
		__attribute__((packed))
#endif
		;
#ifdef _WIN32
#pragma pack(pop)
#endif
	}

	DLLSHUTIL std::ostream &operator<<(std::ostream &out, const pragma::util::Version &version);
}
