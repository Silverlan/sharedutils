// SPDX-FileCopyrightText: (c) 2019 Silverlan <opensource@pragma-engine.com>
// SPDX-License-Identifier: MIT

module;

module pragma.util;

import :property;

using namespace util;

util::BoolProperty::BoolProperty() : SimpleProperty<BoolProperty, bool>{} {}
util::BoolProperty::BoolProperty(const bool &init) : SimpleProperty<BoolProperty, bool>{init} {}
util::BoolProperty::BoolProperty(const BoolProperty &prop) : SimpleProperty<BoolProperty, bool>{prop} {}

void util::BaseProperty::AddCallback(const CallbackHandle &hCallback) { m_callbacks.push_back(hCallback); }

void util::BoolProperty::ApplyValue(const bool &newValue)
{
	m_value = newValue != 0; // Make absolutely sure the value is either 1 or 0
}

util::BoolProperty &util::BoolProperty::operator=(const bool &val)
{
	SimpleProperty<BoolProperty, bool>::operator=(val);
	return *this;
}
util::BoolProperty &util::BoolProperty::operator=(const BoolProperty &other)
{
	SimpleProperty<BoolProperty, bool>::operator=(other);
	return *this;
}
