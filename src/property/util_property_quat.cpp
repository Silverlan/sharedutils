// SPDX-FileCopyrightText: (c) 2019 Silverlan <opensource@pragma-engine.com>
// SPDX-License-Identifier: MIT

#include "sharedutils/property/util_property_quat.hpp"

using namespace util;

QuatProperty::QuatProperty(const Quat &v) : SimpleVectorProperty<QuatProperty, Quat>::SimpleVectorProperty(v) {}
QuatProperty::QuatProperty(float w, float x, float y, float z) : SimpleVectorProperty<QuatProperty, Quat>::SimpleVectorProperty() { m_value = {w, x, y, z}; }

QuatProperty &QuatProperty::operator/=(float f)
{
	this->m_value /= f;
	return *this;
}
QuatProperty &QuatProperty::operator*=(float f)
{
	this->m_value *= f;
	return *this;
}
QuatProperty &QuatProperty::operator*=(const Quat &other)
{
	this->m_value *= other;
	return *this;
}
