// SPDX-FileCopyrightText: (c) 2021 Silverlan <opensource@pragma-engine.com>
// SPDX-License-Identifier: MIT

module;

export module pragma.util:magic_enum_util;

export import magic_enum;
export import std.compat;

export namespace ustring {
	template<typename TEnum>
	constexpr TEnum string_to_enum(const std::string_view &strVal, TEnum defaultVal)
	{
		constexpr auto &enumNames = magic_enum::enum_names<TEnum>();
		for(auto i = decltype(enumNames.size()) {0u}; i < enumNames.size(); ++i) {
			if(iequals(enumNames[i], strVal))
				return magic_enum::enum_value<TEnum>(i);
		}
		return defaultVal;
	}
};
