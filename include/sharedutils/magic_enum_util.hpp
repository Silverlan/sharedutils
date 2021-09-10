/* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at http://mozilla.org/MPL/2.0/.
*
* Copyright (c) 2021 Silverlan
*/

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
