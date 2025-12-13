// SPDX-FileCopyrightText: (c) 2023 Silverlan <opensource@pragma-engine.com>
// SPDX-License-Identifier: MIT

module;

module pragma.util;

import :property_transform;

using namespace pragma::util;

TransformProperty::TransformProperty() : SimpleProperty<TransformProperty, math::Transform>() {}
TransformProperty::TransformProperty(const math::Transform &t) : SimpleProperty<TransformProperty, math::Transform>(t) {}
TransformProperty::TransformProperty(const Vector3 &pos, const Quat &rot) : SimpleProperty<TransformProperty, math::Transform>() { m_value = {pos, rot}; }
TransformProperty &TransformProperty::operator*=(const math::Transform &other)
{
	m_value = m_value * other;
	return *this;
}

ScaledTransformProperty::ScaledTransformProperty() : SimpleProperty<ScaledTransformProperty, math::ScaledTransform>() {}
ScaledTransformProperty::ScaledTransformProperty(const math::ScaledTransform &t) : SimpleProperty<ScaledTransformProperty, math::ScaledTransform>(t) {}
ScaledTransformProperty::ScaledTransformProperty(const Vector3 &pos, const Quat &rot, const Vector3 &scale) : SimpleProperty<ScaledTransformProperty, math::ScaledTransform>() { m_value = {pos, rot, scale}; }
ScaledTransformProperty &ScaledTransformProperty::operator*=(const math::ScaledTransform &other)
{
	m_value = m_value * other;
	return *this;
}
