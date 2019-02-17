/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef __UTIL_PROPERTY_EULER_ANGLES_HPP__
#define __UTIL_PROPERTY_EULER_ANGLES_HPP__

#include "util_property.hpp"
#include <mathutil/color.h>

#pragma warning(push)
#pragma warning(disable : 4251)
namespace util
{
	class DLLSHUTIL EulerAnglesProperty:
		public SimpleProperty<EulerAnglesProperty,EulerAngles>
	{
	public:
		EulerAnglesProperty();
		EulerAnglesProperty(const EulerAngles &col);
		EulerAnglesProperty(float p,float y,float r);
		EulerAnglesProperty(const std::string &str);
		EulerAnglesProperty &operator/=(float f);
		EulerAnglesProperty &operator*=(float f);
		EulerAnglesProperty &operator+=(const EulerAngles &other);
		EulerAnglesProperty &operator-=(const EulerAngles &other);
		using SimpleProperty<EulerAnglesProperty,EulerAngles>::operator=;
	};
	using PEulerAnglesProperty = std::shared_ptr<EulerAnglesProperty>;
};
#pragma warning(pop)

#endif
