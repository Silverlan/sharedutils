// SPDX-FileCopyrightText: (c) 2023 Silverlan <opensource@pragma-engine.com>
// SPDX-License-Identifier: MIT

#ifndef __UTIL_PROPERTY_TRANSFORM_HPP__
#define __UTIL_PROPERTY_TRANSFORM_HPP__

#include "util_property.hpp"
#include <mathutil/transform.hpp>

#pragma warning(push)
#pragma warning(disable : 4251)
namespace util {
	class DLLSHUTIL TransformProperty : public SimpleProperty<TransformProperty, umath::Transform> {
	  public:
		TransformProperty();
		TransformProperty(const umath::Transform &t);
		TransformProperty(const Vector3 &pos, const Quat &rot);
		TransformProperty &operator*=(const umath::Transform &other);
		using SimpleProperty<TransformProperty, umath::Transform>::operator=;
	};
	using PTransformProperty = std::shared_ptr<TransformProperty>;

	class DLLSHUTIL ScaledTransformProperty : public SimpleProperty<ScaledTransformProperty, umath::ScaledTransform> {
	  public:
		ScaledTransformProperty();
		ScaledTransformProperty(const umath::ScaledTransform &t);
		ScaledTransformProperty(const Vector3 &pos, const Quat &rot, const Vector3 &scale);
		ScaledTransformProperty &operator*=(const umath::ScaledTransform &other);
		using SimpleProperty<ScaledTransformProperty, umath::ScaledTransform>::operator=;
	};
	using PScaledTransformProperty = std::shared_ptr<ScaledTransformProperty>;
};
#pragma warning(pop)

#endif
