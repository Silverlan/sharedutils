// SPDX-FileCopyrightText: (c) 2019 Silverlan <opensource@pragma-engine.com>
// SPDX-License-Identifier: MIT

module;

module pragma.util;

import :enum_register;

uint32_t util::EnumRegister::RegisterEnum(const std::string &name)
{
	if(name.empty())
		return INVALID_ENUM;
	std::scoped_lock lock {m_enumMutex};
	auto it = std::find(m_enums.begin(), m_enums.end(), name);
	if(it != m_enums.end())
		return it - m_enums.begin();
	if(m_enums.size() == m_enums.capacity())
		m_enums.reserve(m_enums.size() + 100);
	m_enums.push_back(name);
	for(auto it = m_onRegisterCallbacks.begin(); it != m_onRegisterCallbacks.end();) {
		auto &hCb = *it;
		if(hCb.IsValid() == false)
			it = m_onRegisterCallbacks.erase(it);
		else {
			hCb(std::ref<const std::string>(name), static_cast<uint32_t>(m_enums.size() - 1));
			++it;
		}
	}
	return m_enums.size() - 1;
}
bool util::EnumRegister::GetEnumValue(const std::string &name, uint32_t &val) const
{
	std::scoped_lock lock {m_enumMutex};
	auto it = std::find(m_enums.begin(), m_enums.end(), name);
	if(it == m_enums.end())
		return false;
	val = it - m_enums.begin();
	return true;
}
uint32_t util::EnumRegister::GetEnumValue(const std::string &name) const
{
	auto r = INVALID_ENUM;
	GetEnumValue(name, r);
	return r;
}
const std::string *util::EnumRegister::GetEnumName(uint32_t val) const
{
	std::scoped_lock lock {m_enumMutex};
	if(val >= m_enums.size())
		return nullptr;
	return &m_enums.at(val);
}
const std::vector<std::string> &util::EnumRegister::GetEnums() const { return m_enums; }
void util::EnumRegister::Lock() { m_enumMutex.lock(); }
void util::EnumRegister::Unlock() { m_enumMutex.unlock(); }
CallbackHandle util::EnumRegister::CallOnRegister(const std::function<void(std::reference_wrapper<const std::string>, uint32_t)> &f)
{
	std::scoped_lock lock {m_enumMutex};
	m_onRegisterCallbacks.push_back(FunctionCallback<void, std::reference_wrapper<const std::string>, uint32_t>::Create(f));
	return m_onRegisterCallbacks.back();
}
