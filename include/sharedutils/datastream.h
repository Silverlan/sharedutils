/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef __DATASTREAM_H__
#define __DATASTREAM_H__

#include <string>
#include <memory>
#include <vector>
#include <cstddef>
#include <cstring>
#include "utildefinitions.h"
#include <mathutil/umath.h>
class DataStreamBase;
template<class TDataStreamBase>
	class TDataStream
{
public:
	TDataStream();
	TDataStream(uint32_t size);
	TDataStream(void *data,uint32_t size);
	TDataStream(const TDataStream &o);
	TDataStream(std::nullptr_t t);
	const TDataStreamBase *operator->() const;
	TDataStreamBase *operator->();
	const TDataStreamBase &operator*() const;
	TDataStreamBase &operator*();
	bool operator==(const TDataStream &p) const;
	bool operator!=(const TDataStream &p) const;
    bool operator==(const std::nullptr_t t) const;
    bool operator!=(const std::nullptr_t t) const;
	bool IsValid() const;
	const TDataStreamBase *get() const;
	TDataStreamBase *get();

	template<class T>
		DataStreamBase &operator<<(T *v);
	template<class T>
		DataStreamBase &operator<<(const T &v);
	DataStreamBase &operator<<(const std::string &str);
	DataStreamBase &operator<<(const char *str);
	template<class T>
		DataStreamBase &operator>>(T &v);
	DataStreamBase &operator>>(std::string &str);
private:
	std::shared_ptr<TDataStreamBase> m_baseStreamObject = nullptr;
};

template<class TDataStreamBase>
	template<class T>
		DataStreamBase &TDataStream<TDataStreamBase>::operator<<(T *v) {return (*this->get())<<v;}
template<class TDataStreamBase>
	template<class T>
		DataStreamBase &TDataStream<TDataStreamBase>::operator<<(const T &v) {return (*this->get())<<v;}
template<class TDataStreamBase>
	template<class T>
		DataStreamBase &TDataStream<TDataStreamBase>::operator>>(T &v) {return (*this->get())>>v;}

template<class TDataStreamBase>
	TDataStream<TDataStreamBase>::TDataStream(const TDataStream &o)
		: m_baseStreamObject{o.m_baseStreamObject}
{}
template<class TDataStreamBase>
	TDataStream<TDataStreamBase>::TDataStream()
		: m_baseStreamObject{new TDataStreamBase}
{
	(*this)->Resize(1);
}
template<class TDataStreamBase>
	TDataStream<TDataStreamBase>::TDataStream(uint32_t size)
		: m_baseStreamObject{new TDataStreamBase(size)}
{
	(*this)->Resize(size);
}
template<class TDataStreamBase>
	TDataStream<TDataStreamBase>::TDataStream(void *data,uint32_t size)
		: m_baseStreamObject{new TDataStreamBase(data,size)}
{}
template<class TDataStreamBase>
        TDataStream<TDataStreamBase>::TDataStream(std::nullptr_t)
		: m_baseStreamObject{nullptr}
{}
template<class TDataStreamBase>
	bool TDataStream<TDataStreamBase>::IsValid() const {return (this->get() == nullptr) ? false : true;}
	
template<class TDataStreamBase>
	const TDataStreamBase *TDataStream<TDataStreamBase>::get() const
{
	return const_cast<TDataStream<TDataStreamBase>*>(this)->get();
}
template<class TDataStreamBase>
	TDataStreamBase *TDataStream<TDataStreamBase>::get()
{
	return m_baseStreamObject.get();
}
template<class TDataStreamBase>
	const TDataStreamBase *TDataStream<TDataStreamBase>::operator->() const
{
	return const_cast<TDataStream<TDataStreamBase>*>(this)->operator->();
}
template<class TDataStreamBase>
	TDataStreamBase *TDataStream<TDataStreamBase>::operator->() {return this->get();}
template<class TDataStreamBase>
	const TDataStreamBase &TDataStream<TDataStreamBase>::operator*() const
{
	return const_cast<TDataStream<TDataStreamBase>*>(this)->operator*();
}
template<class TDataStreamBase>
	TDataStreamBase &TDataStream<TDataStreamBase>::operator*()
{
	return *m_baseStreamObject;
}
template<class TDataStreamBase>
	bool TDataStream<TDataStreamBase>::operator==(const TDataStream &p) const {return (this->get() == p.get()) ? true : false;}
template<class TDataStreamBase>
	bool TDataStream<TDataStreamBase>::operator!=(const TDataStream &p) const {return (this->get() != p.get()) ? true : false;}
template<class TDataStreamBase>
        bool TDataStream<TDataStreamBase>::operator==(const std::nullptr_t) const {return !IsValid() ? true : false;}
template<class TDataStreamBase>
        bool TDataStream<TDataStreamBase>::operator!=(const std::nullptr_t) const {return IsValid() ? true : false;}

template<class TDataStreamBase>
	DataStreamBase &TDataStream<TDataStreamBase>::operator<<(const std::string &str) {return (*this->get())<<str;}
template<class TDataStreamBase>
	DataStreamBase &TDataStream<TDataStreamBase>::operator<<(const char *str) {return (*this->get())<<str;}
template<class TDataStreamBase>
	DataStreamBase &TDataStream<TDataStreamBase>::operator>>(std::string &str) {return (*this->get())>>str;}

class DLLSHUTIL DataStream
	: public TDataStream<DataStreamBase>
{
public:
	using TDataStream<DataStreamBase>::TDataStream;
};

#pragma warning(push)
#pragma warning(disable : 4251)
class DLLSHUTIL DataStreamBase
{
public:
	template<class> friend class TDataStream;
protected:
	DataStreamBase();
	DataStreamBase(uint32_t size,uint32_t headerSize=0);
	DataStreamBase(void *data,uint32_t size);
	std::unique_ptr<std::vector<uint8_t>> m_data;
	uint8_t *m_rawData;
	uint32_t m_offset;
	std::unique_ptr<uint32_t> m_size;
	uint32_t m_dataSize;
	uint32_t m_headerSize = 0;
	uint32_t GetClampedSize(uint32_t sz,uint32_t offset) const;
	uint32_t GetClampedSize(uint32_t sz) const;
	bool Eof();
	void AdjustSize(uint32_t offset,uint32_t &sz);
	void SetHeaderSize(uint32_t sz);
public:
	virtual ~DataStreamBase();
	void Write(const uint8_t *c,uint32_t size);
	void Write(const uint8_t *c,uint32_t size,uint32_t pos);
	template<class T>
		void Write(T *v,uint32_t *pos=nullptr);
	template<class T>
		void Write(const T &v,uint32_t *pos=nullptr);
	void WriteString(const std::string &str,Bool bNullTerminated=true);
	void WriteString(const char *str,Bool bNullTerminated=true);
	template<class T>
		DataStreamBase &operator<<(T *v);
	template<class T>
		DataStreamBase &operator<<(const T &v);
	DataStreamBase &operator<<(const std::string &str);
	DataStreamBase &operator<<(const char *str);

	void Read(void *dst,uint32_t size);
	template<class T>
		T Read();
	std::string ReadUntil(const std::string &pattern);
	std::string ReadLine();
	std::string ReadString();
	std::string ReadString(UInt32 len);
	template<class T>
		DataStreamBase &operator>>(T &v);
	DataStreamBase &operator>>(std::string &str);

	void Invalidate();
	uint32_t GetSize() const;
	uint8_t *GetData(bool bIncludeHeaderData=false);
	uint32_t GetDataSize() const;
	uint32_t GetInternalSize() const;
	void SetMessageSize(uint32_t size);
	virtual void Resize(uint32_t sz,bool bForceResize=false);
	void SetOffset(uint32_t offset);
	uint32_t GetOffset() const;
	uint32_t GetHeaderSize() const;
	void SetHeaderData(const void *data);
	void Reserve(uint32_t size);
};
#pragma warning(pop)

template<class T>
	void DataStreamBase::Write(T *v,uint32_t *pos)
{
	if(pos != nullptr)
	{
		Resize(*pos +sizeof(T));
		auto sz = GetClampedSize(sizeof(T),*pos);
		if(sz > 0)
			memcpy(&m_rawData[*pos],v,sz);
		return;
	}
	Write<T>(v,&m_offset);
	m_offset += sizeof(T);
	m_dataSize = umath::max(m_dataSize,m_offset);
}

template<class T>
	void DataStreamBase::Write(const T &v,uint32_t *pos)
{
	if(pos != nullptr)
	{
		Resize(*pos +sizeof(T));
		auto sz = GetClampedSize(sizeof(T),*pos);
		if(sz > 0)
			memcpy(&m_rawData[*pos],&v,sz);
		return;
	}
	Write<T>(v,&m_offset);
	m_offset += sizeof(T);
	m_dataSize = umath::max(m_dataSize,m_offset);
}

template<class T>
	DataStreamBase &DataStreamBase::operator<<(T *v) {Write(v); return *this;}
template<class T>
	DataStreamBase &DataStreamBase::operator<<(const T &v) {Write(v); return *this;}

template<class T>
	T DataStreamBase::Read()
{
	T t;
	Read(&t,sizeof(t));
	return t;
}

template<class T>
	DataStreamBase &DataStreamBase::operator>>(T &v) {Read(&v,sizeof(v)); return *this;}

#endif
