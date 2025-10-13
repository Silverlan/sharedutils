// SPDX-FileCopyrightText: (c) 2023 Silverlan <opensource@pragma-engine.com>
// SPDX-License-Identifier: MIT

module;

#include "sharedutils/utildefinitions.h"

export module pragma.util:property_transform;

export import :property;
export import pragma.math;

export {
	#pragma warning(push)
	#pragma warning(disable : 4251)
	namespace util {
		class DLLSHUTIL TransformProperty : public SimpleProperty<TransformProperty, umath::Transform> {
		public:
			TransformProperty();
			TransformProperty(const umath::Transform &t);
			TransformProperty(const Vector3 &pos, const Quat &rot);
			TransformProperty &operator*=(const umath::Transform &other);

			template<class TProperty, class T>
			TransformProperty &operator=(const T &val) { return static_cast<TransformProperty&>(SimpleProperty<TransformProperty, umath::Transform>::operator=(val)); }
		};
		using PTransformProperty = std::shared_ptr<TransformProperty>;

		class DLLSHUTIL ScaledTransformProperty : public SimpleProperty<ScaledTransformProperty, umath::ScaledTransform> {
		public:
			ScaledTransformProperty();
			ScaledTransformProperty(const umath::ScaledTransform &t);
			ScaledTransformProperty(const Vector3 &pos, const Quat &rot, const Vector3 &scale);
			ScaledTransformProperty &operator*=(const umath::ScaledTransform &other);

			template<class TProperty, class T>
			ScaledTransformProperty &operator=(const T &val) { return static_cast<ScaledTransformProperty&>(SimpleProperty<ScaledTransformProperty, umath::ScaledTransform>::operator=(val)); }
		};
		using PScaledTransformProperty = std::shared_ptr<ScaledTransformProperty>;
	};
	#pragma warning(pop)
}
