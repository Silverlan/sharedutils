// SPDX-FileCopyrightText: (c) 2019 Silverlan <opensource@pragma-engine.com>
// SPDX-License-Identifier: MIT

module;

#include "definitions.hpp"

export module pragma.util:property_string;

export import :property;
export import :property_simple_number;

export {
	namespace pragma::util {
		class DLLSHUTIL StringProperty : public SimpleProperty<StringProperty, std::string> {
		  public:
			StringProperty() : SimpleProperty<StringProperty, std::string> {} {}
			StringProperty(const std::string &init) : SimpleProperty<StringProperty, std::string> {init} {}
			StringProperty(const SimpleProperty<StringProperty, std::string> &prop) : SimpleProperty<StringProperty, std::string> {prop} {}

			// Assignment
			StringProperty &operator+=(const std::string &val);
			StringProperty &operator+=(const SimpleNumberProperty<StringProperty, std::string> &prop);
			StringProperty &operator=(const std::string &val) { return static_cast<StringProperty &>(SimpleProperty<StringProperty, std::string>::operator=(val)); }
			StringProperty &operator=(const StringProperty &other) { return static_cast<StringProperty &>(SimpleProperty<StringProperty, std::string>::operator=(other)); }

			// Arithmetic
			std::string operator+(const std::string &val) const;
			std::string operator+(const SimpleNumberProperty<StringProperty, std::string> &prop) const;

			// Comparison
			bool operator<(const std::string &val) const;
			bool operator<(const SimpleProperty<StringProperty, std::string> &prop) const;
			bool operator>(const std::string &val) const;
			bool operator>(const SimpleProperty<StringProperty, std::string> &prop) const;
			bool operator<=(const std::string &val) const;
			bool operator<=(const SimpleProperty<StringProperty, std::string> &prop) const;
			bool operator>=(const std::string &val) const;
			bool operator>=(const SimpleProperty<StringProperty, std::string> &prop) const;
		};
		using PStringProperty = std::shared_ptr<StringProperty>;
	};
}
