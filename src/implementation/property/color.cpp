// SPDX-FileCopyrightText: (c) 2019 Silverlan <opensource@pragma-engine.com>
// SPDX-License-Identifier: MIT

module;

module pragma.util;

import :property_color;

using namespace pragma::util;

ColorProperty::ColorProperty() : SimpleProperty<ColorProperty, Color>() {}
ColorProperty::ColorProperty(const Color &col) : SimpleProperty<ColorProperty, Color>(col) {}
ColorProperty::ColorProperty(Int16 r, Int16 g, Int16 b, Int16 a) : SimpleProperty<ColorProperty, Color>() { m_value = {r, g, b, a}; }
ColorProperty::ColorProperty(const std::string &str) : SimpleProperty<ColorProperty, Color>() { m_value = {str}; }
ColorProperty::ColorProperty(const Vector3 &v) : SimpleProperty<ColorProperty, Color>() { m_value = {v}; }
ColorProperty::ColorProperty(const Vector4 &v) : SimpleProperty<ColorProperty, Color>() { m_value = {v}; }
ColorProperty &ColorProperty::operator/=(float f)
{
	m_value = m_value / f;
	return *this;
}
ColorProperty &ColorProperty::operator*=(float f)
{
	m_value = m_value * f;
	return *this;
}
ColorProperty &ColorProperty::operator+=(const Color &other)
{
	m_value = m_value + other;
	return *this;
}
ColorProperty &ColorProperty::operator-=(const Color &other)
{
	m_value = m_value - other;
	return *this;
}
