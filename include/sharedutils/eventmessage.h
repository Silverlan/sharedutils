/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef __EVENTMANAGER_H__
#define __EVENTMANAGER_H__

#include <memory>
#include <string>
#include "datastream.h"
#include "utildefinitions.h"

class EventMessage;
class DLLSHUTIL BaseEvent
	: public DataStream
{
public:
	friend EventMessage;
protected:
	BaseEvent();
	int m_value;
	std::unique_ptr<std::string> m_message;
public:
	template<class T>
		BaseEvent(T i,const std::string &msg);
	template<class T>
		BaseEvent(T i);
	std::string GetMessage() const;
	template<class T>
		T GetValue() const;
};

template<class T>
	BaseEvent::BaseEvent(T i)
		: DataStream(),m_value(int(i))
{}

template<class T>
	BaseEvent::BaseEvent(T i,const std::string &msg)
		: DataStream(),m_value(int(i))
{
	m_message = std::unique_ptr<std::string>(new std::string(msg));
}

template<class T>
	T BaseEvent::GetValue() const {return T(m_value);}

/////////////////////////////

class DLLSHUTIL EventMessage
	: public std::shared_ptr<BaseEvent>
{
public:
	template<class T>
		EventMessage(T i);
	template<class T>
		EventMessage(T i,const std::string &msg);
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
		{return (*this)<<v;}
template<class T>
	DataStreamBase &EventMessage::operator<<(const T &v)
		{return (*this->get())<<v;}
template<class T>
	DataStreamBase &EventMessage::operator>>(T &v)
		{return (*this->get())<<v;}

template<class T>
	EventMessage::EventMessage(T i) : std::shared_ptr<BaseEvent>(new BaseEvent(i)) {}
template<class T>
	EventMessage::EventMessage(T i,const std::string &msg) : std::shared_ptr<BaseEvent>(new BaseEvent(i,msg)) {}

template<class T>
	T EventMessage::GetValue() const
{
	auto *ptr = get();
	if(ptr == nullptr)
		return T(0);
	return ptr->GetValue<T>();
}

#endif