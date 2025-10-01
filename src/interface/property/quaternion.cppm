// SPDX-FileCopyrightText: (c) 2019 Silverlan <opensource@pragma-engine.com>
// SPDX-License-Identifier: MIT

module;

#include "sharedutils/utildefinitions.h"
#include "mathutil/uquat.h"

export module pragma.util:property_quaternion;

export import :property_vector;

export {
	namespace util {
		class DLLSHUTIL QuatProperty : public SimpleVectorProperty<QuatProperty, Quat> {
		public:
			QuatProperty() = default;
			QuatProperty(const Quat &v);
			QuatProperty(float w, float x, float y, float z);

			QuatProperty &operator/=(float f);
			QuatProperty &operator*=(float f);
			QuatProperty &operator*=(const Quat &other);

			template<class TProperty, class T>
			QuatProperty &operator=(const T &val) { return static_cast<QuatProperty&>(SimpleProperty<QuatProperty, Quat>::operator=(val)); }
		};
		using PQuatProperty = std::shared_ptr<QuatProperty>;
	};
}
