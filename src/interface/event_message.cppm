// SPDX-FileCopyrightText: (c) 2019 Silverlan <opensource@pragma-engine.com>
// SPDX-License-Identifier: MIT

module;

#include "sharedutils/utildefinitions.h"
#include <memory>
#include <string>

export module pragma.util:event_message;

export import :data_stream;

export {
	class EventMessage;
	class DLLSHUTIL BaseEvent : public DataStream {
	public:
		friend EventMessage;
	protected:
		BaseEvent();
		int m_value;
		std::unique_ptr<std::string> m_message;
	public:
		template<class T>
		BaseEvent(T i, const std::string &msg);
		template<class T>
		BaseEvent(T i);
		std::string GetMessage() const;
		template<class T>
		T GetValue() const;
	};

	template<class T>
	BaseEvent::BaseEvent(T i) : DataStream(), m_value(int(i))
	{
	}

	template<class T>
	BaseEvent::BaseEvent(T i, const std::string &msg) : DataStream(), m_value(int(i))
	{
		m_message = std::unique_ptr<std::string>(new std::string(msg));
	}

	template<class T>
	T BaseEvent::GetValue() const
	{
		return T(m_value);
	}

	/////////////////////////////

	class DLLSHUTIL EventMessage : public std::shared_ptr<BaseEvent> {
	public:
		template<class T>
		EventMessage(T i);
		template<class T>
		EventMessage(T i, const std::string &msg);
		std::string GetMessage() const;
		template<class T>
		T GetValue() const;
		DataStreamBase *operator->();

		template<class T>
		DataStreamBase &operator<<(T *v);
		template<class T>
		DataStreamBase &operator<<(const T &v);
		DataStreamBase &operator<<(const std::string &str);
		DataStreamBase &operator<<(const char *str);
		template<class T>
		DataStreamBase &operator>>(T &v);
		DataStreamBase &operator>>(std::string &str);
	};

	template<class T>
	DataStreamBase &EventMessage::operator<<(T *v)
	{
		return (*this) << v;
	}
	template<class T>
	DataStreamBase &EventMessage::operator<<(const T &v)
	{
		return (*this->get()) << v;
	}
	template<class T>
	DataStreamBase &EventMessage::operator>>(T &v)
	{
		return (*this->get()) << v;
	}

	template<class T>
	EventMessage::EventMessage(T i) : std::shared_ptr<BaseEvent>(new BaseEvent(i))
	{
	}
	template<class T>
	EventMessage::EventMessage(T i, const std::string &msg) : std::shared_ptr<BaseEvent>(new BaseEvent(i, msg))
	{
	}

	template<class T>
	T EventMessage::GetValue() const
	{
		auto *ptr = get();
		if(ptr == nullptr)
			return T(0);
		return ptr->GetValue<T>();
	}
}
