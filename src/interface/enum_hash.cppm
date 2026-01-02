// SPDX-FileCopyrightText: (c) 2019 Silverlan <opensource@pragma-engine.com>
// SPDX-License-Identifier: MIT

export module pragma.util:enum_hash;

export import std.compat;

export {
	namespace pragma::util {
		// Required as a hash when using std::unordered_map with an enum class as key
		struct DLLSHUTIL EnumHash {
			template<typename T>
			std::size_t operator()(T t) const;
		};

		template<typename T>
		std::size_t EnumHash::operator()(T t) const
		{
			return static_cast<std::size_t>(t);
		}
	};
}
