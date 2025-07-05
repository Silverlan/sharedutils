// SPDX-FileCopyrightText: (c) 2021 Silverlan <opensource@pragma-engine.com>
// SPDX-License-Identifier: MIT

#ifndef __MAGIC_ENUM_UTIL_HPP__
#define __MAGIC_ENUM_UTIL_HPP__

#include "sharedutils/magic_enum.hpp"
#include "sharedutils/util_string.h"
namespace ustring
{
    template<typename TEnum>
        constexpr TEnum string_to_enum(const std::string_view &strVal,TEnum defaultVal)
    {
        constexpr auto &enumNames = magic_enum::enum_names<TEnum>();
        for(auto i=decltype(enumNames.size()){0u};i<enumNames.size();++i)
        {
            if(iequals(enumNames[i],strVal))
                return magic_enum::enum_value<TEnum>(i);
        }
        return defaultVal;
    }
};

#endif
