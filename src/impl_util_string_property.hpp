/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef __IMPL_UTIL_STRING_PROPERTY_HPP__
#define __IMPL_UTIL_STRING_PROPERTY_HPP__

#include <string>

#pragma warning(push)
#pragma warning(disable : 4251)
namespace util
{
	class DLLSHUTIL StringProperty
		: public SimpleProperty<StringProperty,std::string>
	{
	public:
		using SimpleProperty<StringProperty,std::string>::SimpleProperty;
		// Assignment
		StringProperty &operator+=(const std::string &val);
		StringProperty &operator+=(const SimpleNumberProperty<StringProperty,std::string> &prop);
		using SimpleProperty<StringProperty,std::string>::operator=;

		// Arithmetic
		std::string operator+(const std::string &val) const;
		std::string operator+(const SimpleNumberProperty<StringProperty,std::string> &prop) const;

		// Comparison
		bool operator<(const std::string &val) const;
		bool operator<(const SimpleProperty<StringProperty,std::string> &prop) const;
		bool operator>(const std::string &val) const;
		bool operator>(const SimpleProperty<StringProperty,std::string> &prop) const;
		bool operator<=(const std::string &val) const;
		bool operator<=(const SimpleProperty<StringProperty,std::string> &prop) const;
		bool operator>=(const std::string &val) const;
		bool operator>=(const SimpleProperty<StringProperty,std::string> &prop) const;
	};
	using PStringProperty = std::shared_ptr<StringProperty>;
};
#pragma warning(pop)

#endif
