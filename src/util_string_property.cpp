/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "sharedutils/property/util_property.hpp"

using namespace util;

// Assignment
StringProperty &StringProperty::operator+=(const std::string &val)
{
	GetValue() += val;
	return *this;
}
StringProperty &StringProperty::operator+=(const SimpleNumberProperty<StringProperty,std::string> &prop) {return *this +=prop.GetValue();}

// Arithmetic
std::string StringProperty::operator+(const std::string &val) const {return GetValue() +val;}
std::string StringProperty::operator+(const SimpleNumberProperty<StringProperty,std::string> &prop) const {return *this +prop.GetValue();}

// Comparison
bool StringProperty::operator<(const std::string &val) const {return GetValue() < val;}
bool StringProperty::operator<(const SimpleProperty<StringProperty,std::string> &prop) const {return *this < prop.GetValue();}
bool StringProperty::operator>(const std::string &val) const {return GetValue() > val;}
bool StringProperty::operator>(const SimpleProperty<StringProperty,std::string> &prop) const {return *this > prop.GetValue();}
bool StringProperty::operator<=(const std::string &val) const {return GetValue() <= val;}
bool StringProperty::operator<=(const SimpleProperty<StringProperty,std::string> &prop) const {return *this <= prop.GetValue();}
bool StringProperty::operator>=(const std::string &val) const {return GetValue() >= val;}
bool StringProperty::operator>=(const SimpleProperty<StringProperty,std::string> &prop) const {return *this >= prop.GetValue();}
