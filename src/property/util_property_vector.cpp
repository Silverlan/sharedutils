/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "sharedutils/property/util_property_vector.h"

using namespace util;

Vector2Property::Vector2Property(const Vector2 &v)
	: SimpleVectorProperty<Vector2Property,Vector2>::SimpleVectorProperty(v)
{}
Vector2Property::Vector2Property(float x,float y)
	: SimpleVectorProperty<Vector2Property,Vector2>::SimpleVectorProperty()
{
	m_value = {x,y};
}

/////////////////////////////

Vector2iProperty::Vector2iProperty(const Vector2i &v)
	: SimpleVectorProperty<Vector2iProperty,Vector2i>::SimpleVectorProperty(v)
{}
Vector2iProperty::Vector2iProperty(int32_t x,int32_t y)
	: SimpleVectorProperty<Vector2iProperty,Vector2i>::SimpleVectorProperty()
{
	m_value = {x,y};
}

/////////////////////////////

Vector3Property::Vector3Property(const Vector3 &v)
	: SimpleVectorProperty<Vector3Property,Vector3>::SimpleVectorProperty(v)
{}
Vector3Property::Vector3Property(float x,float y,float z)
	: SimpleVectorProperty<Vector3Property,Vector3>::SimpleVectorProperty()
{
	m_value = {x,y,z};
}

/////////////////////////////

Vector3iProperty::Vector3iProperty(const Vector3i &v)
	: SimpleVectorProperty<Vector3iProperty,Vector3i>::SimpleVectorProperty(v)
{}
Vector3iProperty::Vector3iProperty(int32_t x,int32_t y,int32_t z)
	: SimpleVectorProperty<Vector3iProperty,Vector3i>::SimpleVectorProperty()
{
	m_value = {x,y,z};
}

/////////////////////////////

Vector4Property::Vector4Property(const Vector4 &v)
	: SimpleVectorProperty<Vector4Property,Vector4>::SimpleVectorProperty(v)
{}
Vector4Property::Vector4Property(float w,float x,float y,float z)
	: SimpleVectorProperty<Vector4Property,Vector4>::SimpleVectorProperty()
{
	m_value = {w,x,y,z};
}

/////////////////////////////

Vector4iProperty::Vector4iProperty(const Vector4i &v)
	: SimpleVectorProperty<Vector4iProperty,Vector4i>::SimpleVectorProperty(v)
{}
Vector4iProperty::Vector4iProperty(int32_t w,int32_t x,int32_t y,int32_t z)
	: SimpleVectorProperty<Vector4iProperty,Vector4i>::SimpleVectorProperty()
{
	m_value = {w,x,y,z};
}
