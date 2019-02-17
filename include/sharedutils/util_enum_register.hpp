/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef __UTIL_ENUM_REGISTER_HPP__
#define __UTIL_ENUM_REGISTER_HPP__

#include "utildefinitions.h"
#include "functioncallback.h"
#include <vector>
#include <cinttypes>

namespace util
{
	class DLLSHUTIL EnumRegister
	{
	public:
		uint32_t RegisterEnum(const std::string &name);
		bool GetEnumValue(const std::string &name,uint32_t &val) const;
		uint32_t GetEnumValue(const std::string &name) const;
		const std::string *GetEnumName(uint32_t val) const;
		const std::vector<std::string> &GetEnums() const;
		CallbackHandle CallOnRegister(const std::function<void(std::reference_wrapper<const std::string>,uint32_t)> &f);
		static const uint32_t InvalidEnum;
	protected:
		std::vector<std::string> m_enums;
		std::vector<CallbackHandle> m_onRegisterCallbacks;
	};
};

#endif
