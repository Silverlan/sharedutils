// SPDX-FileCopyrightText: (c) 2023 Silverlan <opensource@pragma-engine.com>
// SPDX-License-Identifier: MIT

module;

module pragma.util;

import :property_transform;

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
