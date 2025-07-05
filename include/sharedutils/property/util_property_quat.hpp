// SPDX-FileCopyrightText: (c) 2019 Silverlan <opensource@pragma-engine.com>
// SPDX-License-Identifier: MIT

#ifndef __UTIL_PROPERTY_QUAT_HPP__
#define __UTIL_PROPERTY_QUAT_HPP__

#include "util_property_vector.h"

namespace util {
	class DLLSHUTIL QuatProperty : public SimpleVectorProperty<QuatProperty, Quat> {
	  public:
		QuatProperty() = default;
		QuatProperty(const Quat &v);
		QuatProperty(float w, float x, float y, float z);

		QuatProperty &operator/=(float f);
		QuatProperty &operator*=(float f);
		QuatProperty &operator*=(const Quat &other);
		using SimpleVectorProperty<QuatProperty, Quat>::operator=;
	};
	using PQuatProperty = std::shared_ptr<QuatProperty>;
};

#endif
