/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "sharedutils/property/util_property_color.hpp"

using namespace util;

ColorProperty::ColorProperty()
	: SimpleProperty<ColorProperty,Color>()
{}
ColorProperty::ColorProperty(const Color &col)
	: SimpleProperty<ColorProperty,Color>(col)
{}
ColorProperty::ColorProperty(Int16 r,Int16 g,Int16 b,Int16 a)
	: SimpleProperty<ColorProperty,Color>()
{
	m_value = {r,g,b,a};
}
ColorProperty::ColorProperty(const std::string &str)
	: SimpleProperty<ColorProperty,Color>()
{
	m_value = {str};
}
ColorProperty::ColorProperty(const Vector3 &v)
	: SimpleProperty<ColorProperty,Color>()
{
	m_value = {v};
}
ColorProperty::ColorProperty(const Vector4 &v)
	: SimpleProperty<ColorProperty,Color>()
{
	m_value = {v};
}
ColorProperty &ColorProperty::operator/=(float f)
{
	m_value = m_value /f;
	return *this;
}
ColorProperty &ColorProperty::operator*=(float f)
{
	m_value = m_value *f;
	return *this;
}
ColorProperty &ColorProperty::operator+=(const Color &other)
{
	m_value = m_value +other;
	return *this;
}
ColorProperty &ColorProperty::operator-=(const Color &other)
{
	m_value = m_value -other;
	return *this;
}
