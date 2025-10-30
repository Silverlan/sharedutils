// SPDX-FileCopyrightText: (c) 2019 Silverlan <opensource@pragma-engine.com>
// SPDX-License-Identifier: MIT

module;


module pragma.util;

import :property_string;

using namespace util;

// Assignment
StringProperty &StringProperty::operator+=(const std::string &val)
{
	GetValue() += val;
	return *this;
}
StringProperty &StringProperty::operator+=(const SimpleNumberProperty<StringProperty, std::string> &prop) { return *this += prop.GetValue(); }

// Arithmetic
std::string StringProperty::operator+(const std::string &val) const { return GetValue() + val; }
std::string StringProperty::operator+(const SimpleNumberProperty<StringProperty, std::string> &prop) const { return *this + prop.GetValue(); }

// Comparison
bool StringProperty::operator<(const std::string &val) const { return GetValue() < val; }
bool StringProperty::operator<(const SimpleProperty<StringProperty, std::string> &prop) const { return *this < prop.GetValue(); }
bool StringProperty::operator>(const std::string &val) const { return GetValue() > val; }
bool StringProperty::operator>(const SimpleProperty<StringProperty, std::string> &prop) const { return *this > prop.GetValue(); }
bool StringProperty::operator<=(const std::string &val) const { return GetValue() <= val; }
bool StringProperty::operator<=(const SimpleProperty<StringProperty, std::string> &prop) const { return *this <= prop.GetValue(); }
bool StringProperty::operator>=(const std::string &val) const { return GetValue() >= val; }
bool StringProperty::operator>=(const SimpleProperty<StringProperty, std::string> &prop) const { return *this >= prop.GetValue(); }
