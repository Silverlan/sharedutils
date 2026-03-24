// SPDX-FileCopyrightText: (c) 2026 Silverlan <opensource@pragma-engine.com>
// SPDX-License-Identifier: MIT

module;

module pragma.util;

import :global_string_table;

static pragma::util::GlobalStringTable *g_globalStringTable = nullptr;
void pragma::util::set_global_string_table(GlobalStringTable &table) {
	g_globalStringTable = &table;
}

pragma::util::GlobalStringTable &pragma::util::get_global_string_table()
{
	if(g_globalStringTable)
		return *g_globalStringTable;
	static pragma::util::GlobalStringTable defaultTable;
	return defaultTable;
}

pragma::util::GlobalStringTable::~GlobalStringTable()
{
	std::unique_lock lock {m_mutex};
	for(auto &pair : m_strings)
		delete[] pair.second;
}

const char *pragma::util::GlobalStringTable::RegisterString(const std::string_view &str)
{
	// TODO: Once https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2018/p0919r2.html is part of the C++ standard, we can use string_view directly for the map
	// lookup. Until then we'll convert it to a std::string.
	return RegisterString(std::string {str});
}

const char *pragma::util::GlobalStringTable::RegisterString(const std::string &str)
{
	std::unique_lock lock {m_mutex};
	auto it = m_strings.find(str);
	if(it != m_strings.end())
		return it->second;
	else {
		char *registrationId = new char[str.size() + 1];
		std::copy(str.begin(), str.end(), registrationId);
		registrationId[str.size()] = '\0';
		m_strings.emplace(str, registrationId);
		return registrationId;
	}
}

const char *pragma::util::GlobalStringTable::RegisterString(const char *str)
{
	std::unique_lock lock {m_mutex};
	auto it = m_strings.find(str);
	if(it != m_strings.end())
		return it->second;
	else {
		auto len = strlen(str);
		char *registrationId = new char[len + 1];
		strcpy(registrationId, str);
		registrationId[len] = '\0';
		m_strings.emplace(str, registrationId);
		return registrationId;
	}
}

const char *pragma::util::register_global_string(const std::string &str) { return get_global_string_table().RegisterString(str); }
const char *pragma::util::register_global_string(const std::string_view &str) { return get_global_string_table().RegisterString(str); }
const char *pragma::util::register_global_string(const char *str) { return get_global_string_table().RegisterString(str); }

pragma::util::GString::GString() {}
pragma::util::GString::GString(const char *str) : str {str} {}
pragma::util::GString::GString(const std::string &str) : str {register_global_string(str)} {}
pragma::util::GString::GString(const std::string_view &str) : str {register_global_string(str)} {}
pragma::util::GString::GString(const GString &other) : str {other.str} {}

pragma::util::GString &pragma::util::GString::operator=(const char *str)
{
	this->str = str;
	return *this;
}
pragma::util::GString &pragma::util::GString::operator=(const std::string &str)
{
	this->str = register_global_string(str);
	return *this;
}
pragma::util::GString &pragma::util::GString::operator=(const std::string_view &str)
{
	this->str = register_global_string(str);
	return *this;
}
pragma::util::GString &pragma::util::GString::operator=(const GString &str)
{
	this->str = str.str;
	return *this;
}
const char *pragma::util::GString::operator*() const { return str; }
const char *pragma::util::GString::c_str() const { return str; }
bool pragma::util::GString::empty() const { return str == nullptr || str[0] == '\0'; }
size_t pragma::util::GString::length() const { return str ? strlen(str) : 0; }

pragma::util::GString::operator const char *() const { return str ? str : ""; }
pragma::util::GString::operator std::string() const { return str ? str : ""; }
pragma::util::GString::operator std::string_view() const { return str ? str : ""; }
pragma::util::GString::operator bool() const { return str ? true : false; }

bool pragma::util::GString::operator==(const char *str) const
{
	if(this->str == nullptr && str == nullptr) {
		return true;
	}
	if(this->str == nullptr || str == nullptr) {
		return false;
	}
	return strcmp(this->str, str) == 0;
}
bool pragma::util::GString::operator!=(const char *str) const { return !(*this == str); }
bool pragma::util::GString::operator==(const std::string &str) const { return *this == str.c_str(); }
bool pragma::util::GString::operator!=(const std::string &str) const { return !(*this == str); }
bool pragma::util::GString::operator==(const std::string_view &str) const { return *this == str.data(); }
bool pragma::util::GString::operator!=(const std::string_view &str) const { return !(*this == str); }
bool pragma::util::GString::operator==(const GString &other) const { return *this == other.str; }
bool pragma::util::GString::operator!=(const GString &other) const { return !(*this == other); }

std::ostream &pragma::util::operator<<(std::ostream &stream, const pragma::util::GString &str)
{
	if(!str)
		stream << 0;
	else
		stream << str.c_str();
	return stream;
}
