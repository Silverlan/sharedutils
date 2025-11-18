// SPDX-FileCopyrightText: (c) 2019 Silverlan <opensource@pragma-engine.com>
// SPDX-License-Identifier: MIT

module;

module pragma.util;

import :event_message;

std::string EventMessage::GetMessage() const
{
	auto *ptr = get();
	if(ptr == nullptr)
		return "";
	return ptr->GetMessage();
}
util::DataStreamBase *EventMessage::operator->()
{
	auto *ptr = get();
	return ptr->get();
}

util::DataStreamBase &EventMessage::operator<<(const std::string &str) { return (*this->get()) << str; }
util::DataStreamBase &EventMessage::operator<<(const char *str) { return (*this->get()) << str; }
util::DataStreamBase &EventMessage::operator>>(std::string &str) { return (*this->get()) << str; }

///////////////////////

BaseEvent::BaseEvent() : DataStream() {}

std::string BaseEvent::GetMessage() const
{
	auto *ptr = m_message.get();
	if(ptr == nullptr)
		return "";
	return *ptr;
}
