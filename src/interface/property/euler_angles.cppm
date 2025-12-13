// SPDX-FileCopyrightText: (c) 2019 Silverlan <opensource@pragma-engine.com>
// SPDX-License-Identifier: MIT

module;

#include "definitions.hpp"

export module pragma.util:property_euler_angles;

export import :property;
export import pragma.math;

export {
#pragma warning(push)
#pragma warning(disable : 4251)
	namespace pragma::util {
		class DLLSHUTIL EulerAnglesProperty : public SimpleProperty<EulerAnglesProperty, EulerAngles> {
		  public:
			EulerAnglesProperty();
			EulerAnglesProperty(const EulerAngles &col);
			EulerAnglesProperty(float p, float y, float r);
			EulerAnglesProperty(const std::string &str);
			EulerAnglesProperty &operator/=(float f);
			EulerAnglesProperty &operator*=(float f);
			EulerAnglesProperty &operator+=(const EulerAngles &other);
			EulerAnglesProperty &operator-=(const EulerAngles &other);

			template<class TProperty, class T>
			EulerAnglesProperty &operator=(const T &val)
			{
				return static_cast<EulerAnglesProperty &>(SimpleProperty<EulerAnglesProperty, EulerAngles>::operator=(val));
			}
		};
		using PEulerAnglesProperty = std::shared_ptr<EulerAnglesProperty>;
	};
#pragma warning(pop)
}
