// SPDX-FileCopyrightText: (c) 2023 Silverlan <opensource@pragma-engine.com>
// SPDX-License-Identifier: MIT

module;

#include "definitions.hpp"

export module pragma.util:property_transform;

export import :property;
export import pragma.math;

export {
#pragma warning(push)
#pragma warning(disable : 4251)
	namespace pragma::util {
		class DLLSHUTIL TransformProperty : public SimpleProperty<TransformProperty, pragma::math::Transform> {
		  public:
			TransformProperty();
			TransformProperty(const pragma::math::Transform &t);
			TransformProperty(const Vector3 &pos, const Quat &rot);
			TransformProperty &operator*=(const pragma::math::Transform &other);

			template<class TProperty, class T>
			TransformProperty &operator=(const T &val)
			{
				return static_cast<TransformProperty &>(SimpleProperty<TransformProperty, pragma::math::Transform>::operator=(val));
			}
		};
		using PTransformProperty = std::shared_ptr<TransformProperty>;

		class DLLSHUTIL ScaledTransformProperty : public SimpleProperty<ScaledTransformProperty, pragma::math::ScaledTransform> {
		  public:
			ScaledTransformProperty();
			ScaledTransformProperty(const pragma::math::ScaledTransform &t);
			ScaledTransformProperty(const Vector3 &pos, const Quat &rot, const Vector3 &scale);
			ScaledTransformProperty &operator*=(const pragma::math::ScaledTransform &other);

			template<class TProperty, class T>
			ScaledTransformProperty &operator=(const T &val)
			{
				return static_cast<ScaledTransformProperty &>(SimpleProperty<ScaledTransformProperty, pragma::math::ScaledTransform>::operator=(val));
			}
		};
		using PScaledTransformProperty = std::shared_ptr<ScaledTransformProperty>;
	};
#pragma warning(pop)
}
