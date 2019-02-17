/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "sharedutils/util_enum_register.hpp"
#include <limits>
#include <algorithm>

decltype(util::EnumRegister::InvalidEnum) util::EnumRegister::InvalidEnum = std::numeric_limits<decltype(util::EnumRegister::InvalidEnum)>::max();
uint32_t util::EnumRegister::RegisterEnum(const std::string &name)
{
	if(name.empty())
		return InvalidEnum;
	auto val = GetEnumValue(name);
	if(val != InvalidEnum)
		return val;
	if(m_enums.size() == m_enums.capacity())
		m_enums.reserve(m_enums.size() +100);
	m_enums.push_back(name);
	for(auto it=m_onRegisterCallbacks.begin();it!=m_onRegisterCallbacks.end();)
	{
		auto &hCb = *it;
		if(hCb.IsValid() == false)
			it = m_onRegisterCallbacks.erase(it);
		else
		{
			hCb(std::ref<const std::string>(name),static_cast<uint32_t>(m_enums.size() -1));
			++it;
		}
	}
	return m_enums.size() -1;
}
bool util::EnumRegister::GetEnumValue(const std::string &name,uint32_t &val) const
{
	auto it = std::find(m_enums.begin(),m_enums.end(),name);
	if(it == m_enums.end())
		return false;
	val = it -m_enums.begin();
	return true;
}
uint32_t util::EnumRegister::GetEnumValue(const std::string &name) const
{
	auto r = InvalidEnum;
	GetEnumValue(name,r);
	return r;
}
const std::string *util::EnumRegister::GetEnumName(uint32_t val) const
{
	if(val >= m_enums.size())
		return nullptr;
	return &m_enums.at(val);
}
const std::vector<std::string> &util::EnumRegister::GetEnums() const {return m_enums;}
CallbackHandle util::EnumRegister::CallOnRegister(const std::function<void(std::reference_wrapper<const std::string>,uint32_t)> &f)
{
	m_onRegisterCallbacks.push_back(FunctionCallback<void,std::reference_wrapper<const std::string>,uint32_t>::Create(f));
	return m_onRegisterCallbacks.back();
}
