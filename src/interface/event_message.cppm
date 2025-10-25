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
	class DLLSHUTIL BaseEvent : public util::DataStream {
	public:
		friend EventMessage;
	protected:
		BaseEvent();
		int m_value;
		std::unique_ptr<std::string> m_message;
	public:
		template<class T>
		BaseEvent(T i, const std::string &msg) : util::DataStream(), m_value(int(i))
		{
			m_message = std::unique_ptr<std::string>(new std::string(msg));
		}
		template<class T>
		BaseEvent(T i) : util::DataStream(), m_value(int(i))
		{
		}
		std::string GetMessage() const;
		template<class T>
		T GetValue() const
		{
			return T(m_value);
		}
	};

	/////////////////////////////

	class DLLSHUTIL EventMessage : public std::shared_ptr<BaseEvent> {
	public:
		template<class T>
		EventMessage(T i) : std::shared_ptr<BaseEvent>(new BaseEvent(i))
		{
		}
		template<class T>
		EventMessage(T i, const std::string &msg) : std::shared_ptr<BaseEvent>(new BaseEvent(i, msg))
		{
		}
		std::string GetMessage() const;
		template<class T>
		T GetValue() const
		{
			auto *ptr = get();
			if(ptr == nullptr)
				return T(0);
			return ptr->GetValue<T>();
		}
		util::DataStreamBase *operator->();

		template<class T>
		util::DataStreamBase &operator<<(T *v)
		{
			return (*this) << v;
		}
		template<class T>
		util::DataStreamBase &operator<<(const T &v)
		{
			return (*this->get()) << v;
		}
		util::DataStreamBase &operator<<(const std::string &str);
		util::DataStreamBase &operator<<(const char *str);
		template<class T>
		util::DataStreamBase &operator>>(T &v)
		{
			return (*this->get()) << v;
		}
		util::DataStreamBase &operator>>(std::string &str);
	};
}
