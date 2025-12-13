// SPDX-FileCopyrightText: (c) 2019 Silverlan <opensource@pragma-engine.com>
// SPDX-License-Identifier: MIT

module;

module pragma.util;

import :property_euler_angles;

using namespace pragma::util;

EulerAnglesProperty::EulerAnglesProperty() : SimpleProperty<EulerAnglesProperty, EulerAngles>() {}
EulerAnglesProperty::EulerAnglesProperty(const EulerAngles &col) : SimpleProperty<EulerAnglesProperty, EulerAngles>(col) {}
EulerAnglesProperty::EulerAnglesProperty(float p, float y, float r) : SimpleProperty<EulerAnglesProperty, EulerAngles>() { m_value = {p, y, r}; }
EulerAnglesProperty::EulerAnglesProperty(const std::string &str) : SimpleProperty<EulerAnglesProperty, EulerAngles>() { m_value = {str}; }
EulerAnglesProperty &EulerAnglesProperty::operator/=(float f)
{
	m_value = m_value / f;
	return *this;
}
EulerAnglesProperty &EulerAnglesProperty::operator*=(float f)
{
	m_value = m_value * f;
	return *this;
}
EulerAnglesProperty &EulerAnglesProperty::operator+=(const EulerAngles &other)
{
	m_value = m_value + other;
	return *this;
}
EulerAnglesProperty &EulerAnglesProperty::operator-=(const EulerAngles &other)
{
	m_value = m_value - other;
	return *this;
}
