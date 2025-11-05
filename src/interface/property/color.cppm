// SPDX-FileCopyrightText: (c) 2019 Silverlan <opensource@pragma-engine.com>
// SPDX-License-Identifier: MIT

module;

#include "definitions.hpp"

export module pragma.util:property_color;

export import :property;
export import pragma.math;

export {
#pragma warning(push)
#pragma warning(disable : 4251)
	namespace util {
		class DLLSHUTIL ColorProperty : public SimpleProperty<ColorProperty, Color> {
		  public:
			ColorProperty();
			ColorProperty(const Color &col);
			ColorProperty(Int16 r, Int16 g, Int16 b, Int16 a = 255);
			ColorProperty(const std::string &str);
			ColorProperty(const Vector3 &v);
			ColorProperty(const Vector4 &v);
			ColorProperty &operator/=(float f);
			ColorProperty &operator*=(float f);
			ColorProperty &operator+=(const Color &other);
			ColorProperty &operator-=(const Color &other);

			template<class TProperty, class T>
			ColorProperty &operator=(const T &val)
			{
				return static_cast<ColorProperty &>(SimpleProperty<ColorProperty, Color>::operator=(val));
			}
		};
		using PColorProperty = std::shared_ptr<ColorProperty>;
	};
#pragma warning(pop)
}
