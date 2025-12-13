// SPDX-FileCopyrightText: (c) 2019 Silverlan <opensource@pragma-engine.com>
// SPDX-License-Identifier: MIT

module;

module pragma.util;

import :property;

using namespace pragma::util;

BoolProperty::BoolProperty() : SimpleProperty<BoolProperty, bool> {} {}
BoolProperty::BoolProperty(const bool &init) : SimpleProperty<BoolProperty, bool> {init} {}
BoolProperty::BoolProperty(const BoolProperty &prop) : SimpleProperty<BoolProperty, bool> {prop} {}

void BaseProperty::AddCallback(const CallbackHandle &hCallback) { m_callbacks.push_back(hCallback); }

void BoolProperty::ApplyValue(const bool &newValue)
{
	m_value = newValue != 0; // Make absolutely sure the value is either 1 or 0
}

BoolProperty &BoolProperty::operator=(const bool &val)
{
	SimpleProperty<BoolProperty, bool>::operator=(val);
	return *this;
}
BoolProperty &BoolProperty::operator=(const BoolProperty &other)
{
	SimpleProperty<BoolProperty, bool>::operator=(other);
	return *this;
}
