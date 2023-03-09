/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "sharedutils/property/util_property_transform.hpp"

using namespace util;

TransformProperty::TransformProperty() : SimpleProperty<TransformProperty, umath::Transform>() {}
TransformProperty::TransformProperty(const umath::Transform &t) : SimpleProperty<TransformProperty, umath::Transform>(t) {}
TransformProperty::TransformProperty(const Vector3 &pos, const Quat &rot) : SimpleProperty<TransformProperty, umath::Transform>() { m_value = {pos, rot}; }
TransformProperty &TransformProperty::operator*=(const umath::Transform &other)
{
	m_value = m_value * other;
	return *this;
}

ScaledTransformProperty::ScaledTransformProperty() : SimpleProperty<ScaledTransformProperty, umath::ScaledTransform>() {}
ScaledTransformProperty::ScaledTransformProperty(const umath::ScaledTransform &t) : SimpleProperty<ScaledTransformProperty, umath::ScaledTransform>(t) {}
ScaledTransformProperty::ScaledTransformProperty(const Vector3 &pos, const Quat &rot, const Vector3 &scale) : SimpleProperty<ScaledTransformProperty, umath::ScaledTransform>() { m_value = {pos, rot, scale}; }
ScaledTransformProperty &ScaledTransformProperty::operator*=(const umath::ScaledTransform &other)
{
	m_value = m_value * other;
	return *this;
}
