/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "sharedutils/property/util_property_euler_angles.hpp"

using namespace util;

EulerAnglesProperty::EulerAnglesProperty()
	: SimpleProperty<EulerAnglesProperty,EulerAngles>()
{}
EulerAnglesProperty::EulerAnglesProperty(const EulerAngles &col)
	: SimpleProperty<EulerAnglesProperty,EulerAngles>(col)
{}
EulerAnglesProperty::EulerAnglesProperty(float p,float y,float r)
	: SimpleProperty<EulerAnglesProperty,EulerAngles>()
{
	m_value = {p,y,r};
}
EulerAnglesProperty::EulerAnglesProperty(const std::string &str)
	: SimpleProperty<EulerAnglesProperty,EulerAngles>()
{
	m_value = {str};
}
EulerAnglesProperty &EulerAnglesProperty::operator/=(float f)
{
	m_value = m_value /f;
	return *this;
}
EulerAnglesProperty &EulerAnglesProperty::operator*=(float f)
{
	m_value = m_value *f;
	return *this;
}
EulerAnglesProperty &EulerAnglesProperty::operator+=(const EulerAngles &other)
{
	m_value = m_value +other;
	return *this;
}
EulerAnglesProperty &EulerAnglesProperty::operator-=(const EulerAngles &other)
{
	m_value = m_value -other;
	return *this;
}
