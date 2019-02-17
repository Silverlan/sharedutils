/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef __UTIL_PROPERTY_COLOR_HPP__
#define __UTIL_PROPERTY_COLOR_HPP__

#include "util_property.hpp"
#include <mathutil/color.h>

#pragma warning(push)
#pragma warning(disable : 4251)
namespace util
{
	class DLLSHUTIL ColorProperty:
		public SimpleProperty<ColorProperty,Color>
	{
	public:
		ColorProperty();
		ColorProperty(const Color &col);
		ColorProperty(Int16 r,Int16 g,Int16 b,Int16 a=255);
		ColorProperty(const std::string &str);
		ColorProperty(const Vector3 &v);
		ColorProperty(const Vector4 &v);
		ColorProperty &operator/=(float f);
		ColorProperty &operator*=(float f);
		ColorProperty &operator+=(const Color &other);
		ColorProperty &operator-=(const Color &other);
		using SimpleProperty<ColorProperty,Color>::operator=;
	};
	using PColorProperty = std::shared_ptr<ColorProperty>;
};
#pragma warning(pop)

#endif
