/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "sharedutils/property/util_property_quat.hpp"

using namespace util;

QuatProperty::QuatProperty(const Quat &v)
	: SimpleVectorProperty<QuatProperty,Quat>::SimpleVectorProperty(v)
{}
QuatProperty::QuatProperty(float w,float x,float y,float z)
	: SimpleVectorProperty<QuatProperty,Quat>::SimpleVectorProperty()
{
	m_value = {w,x,y,z};
}

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
