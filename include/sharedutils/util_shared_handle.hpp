/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef __UTIL_SHARED_HANDLE_HPP__
#define __UTIL_SHARED_HANDLE_HPP__

#include "utildefinitions.h"
#include <memory>
#include <functional>

namespace util
{
	class DLLSHUTIL PtrSharedHandleData
	{
	public:
		PtrSharedHandleData(const std::shared_ptr<void> &data);
		void Invalidate();
		void *GetData();
	private:
		std::shared_ptr<void> m_data = nullptr;
	};

	template<typename T>
		class TWeakSharedHandle;
	template<class TSrc,class TDst>
		TWeakSharedHandle<TDst> weak_shared_handle_cast(const TWeakSharedHandle<TSrc> &handle);

	template<typename T>
		class TSharedHandle
	{
	public:
		friend TWeakSharedHandle<T>;
		TSharedHandle(const TSharedHandle<T>&)=default;
		TSharedHandle(const std::shared_ptr<PtrSharedHandleData> &data,T *typedDataPtr);
		TSharedHandle();
		TSharedHandle(nullptr_t);
		TSharedHandle(T *data,const std::function<void(T*)> &customDeleter=nullptr);
		TSharedHandle(const TWeakSharedHandle<T>&)=delete;
		operator TWeakSharedHandle<T>() const;
		bool operator==(nullptr_t) const;
		bool operator!=(nullptr_t) const;
		T *operator->();
		const T *operator->() const;
		T &operator*();
		const T &operator*() const;
		bool IsValid() const;
		bool IsExpired() const;
		const T *Get() const;
		T *Get();
		const T *GetRawPtr() const;
		T *GetRawPtr();
		void Remove();

		const std::shared_ptr<PtrSharedHandleData> &GetInternalData() const;
	private:
		std::shared_ptr<PtrSharedHandleData> m_data = nullptr;
		T *m_typedPtr = nullptr;
	};

	template<class TSrc,class TDst>
		TSharedHandle<TDst> shared_handle_cast(const TSharedHandle<TSrc> &handle);
	template<class T>
		TSharedHandle<T> claim_shared_handle_ownership(const TWeakSharedHandle<T> &wkHandle);

	template<typename T>
		class TWeakSharedHandle
	{
	public:
		template<class TSrc,class TDst>
			friend TWeakSharedHandle<TDst> weak_shared_handle_cast(const TWeakSharedHandle<TSrc> &handle);
		template<class T>
			friend TSharedHandle<T> claim_shared_handle_ownership(const TWeakSharedHandle<T> &wkHandle);

		TWeakSharedHandle(const TWeakSharedHandle<T>&)=default;
		TWeakSharedHandle(const TSharedHandle<T> &sharedHandle);
		TWeakSharedHandle();
		
		TWeakSharedHandle<T> operator=(const TWeakSharedHandle<T> &other);
		TWeakSharedHandle<T> operator=(const TSharedHandle<T> &other);
		T *operator->();
		const T *operator->() const;
		T &operator*();
		const T &operator*() const;
		bool IsValid() const;
		bool IsExpired() const;
		const T *Get() const;
		T *Get();
		const T *GetRawPtr() const;
		T *GetRawPtr();
		void Remove();

		std::shared_ptr<PtrSharedHandleData> GetInternalData() const;
	private:
		TWeakSharedHandle(const std::shared_ptr<PtrSharedHandleData> &data,T *typedDataPtr);
		std::weak_ptr<PtrSharedHandleData> m_data = {};
		T *m_typedPtr = nullptr;
	};
};

template<typename T>
	util::TSharedHandle<T>::TSharedHandle()
		: m_data{nullptr},m_typedPtr{nullptr}
{}

template<typename T>
	util::TSharedHandle<T>::TSharedHandle(nullptr_t)
		: m_data{nullptr},m_typedPtr{nullptr}
{}

template<typename T>
	util::TSharedHandle<T>::TSharedHandle(T *data,const std::function<void(T*)> &customDeleter)
		: m_data{std::make_shared<PtrSharedHandleData>(customDeleter ? std::shared_ptr<T>{data,customDeleter} : std::shared_ptr<T>{data})},
		m_typedPtr{data}
{}

template<typename T>
	util::TSharedHandle<T>::TSharedHandle(const std::shared_ptr<PtrSharedHandleData> &data,T *typedDataPtr)
		: m_data{data},m_typedPtr{typedDataPtr}
{}
template<typename T>
	util::TSharedHandle<T>::operator util::TWeakSharedHandle<T>() const {return TWeakSharedHandle{*this};}
template<typename T>
	bool util::TSharedHandle<T>::operator==(nullptr_t) const {return !operator!=(nullptr);}
template<typename T>
	bool util::TSharedHandle<T>::operator!=(nullptr_t) const {return IsValid();}
template<typename T>
	T *util::TSharedHandle<T>::operator->() {return Get();}
template<typename T>
	const T *util::TSharedHandle<T>::operator->() const {return const_cast<util::TSharedHandle<T>*>(this)->operator->();}
template<typename T>
	T &util::TSharedHandle<T>::operator*() {return *Get();}
template<typename T>
	const T &util::TSharedHandle<T>::operator*() const {return const_cast<util::TSharedHandle<T>*>(this)->operator*();}
template<typename T>
	bool util::TSharedHandle<T>::IsValid() const {return m_data && m_data->GetData() != nullptr;}
template<typename T>
bool util::TSharedHandle<T>::IsExpired() const {return IsValid() == false;}
template<typename T>
	const T *util::TSharedHandle<T>::Get() const {return const_cast<TSharedHandle<T>*>(this)->Get();}
template<typename T>
	T *util::TSharedHandle<T>::Get() {return IsValid() ? m_typedPtr : nullptr;}
template<typename T>
	const T *util::TSharedHandle<T>::GetRawPtr() const {return const_cast<TSharedHandle<T>*>(this)->GetRawPtr();}
template<typename T>
	T *util::TSharedHandle<T>::GetRawPtr() {return m_typedPtr;}
template<typename T>
	void util::TSharedHandle<T>::Remove() {m_data->Invalidate();}
	
template<typename T>
	const std::shared_ptr<util::PtrSharedHandleData> &util::TSharedHandle<T>::GetInternalData() const {return m_data;}

//

template<class TSrc,class TDst>
	util::TSharedHandle<TDst> util::shared_handle_cast(const TSharedHandle<TSrc> &handle)
{
	if constexpr (std::is_convertible<TSrc*,TDst*>::value)
		return util::TSharedHandle<TDst>{handle.GetInternalData(),static_cast<TDst*>(const_cast<TSrc*>(handle.Get()))};
	else
		return util::TSharedHandle<TDst>{handle.GetInternalData(),dynamic_cast<TDst*>(const_cast<TSrc*>(handle.Get()))};
}

template<class T>
	util::TSharedHandle<T> util::claim_shared_handle_ownership(const TWeakSharedHandle<T> &wkHandle)
{
	return wkHandle.m_data.expired() ? TSharedHandle<T>{} : TSharedHandle<T>{wkHandle.m_data.lock(),wkHandle.m_typedPtr};
}

//////////////////////
	
template<typename T>
	util::TWeakSharedHandle<T>::TWeakSharedHandle()
		: m_data{},m_typedPtr{nullptr}
{}

template<typename T>
	util::TWeakSharedHandle<T>::TWeakSharedHandle(const TSharedHandle<T> &sharedHandle)
		: m_data{sharedHandle.GetInternalData()},m_typedPtr{sharedHandle.m_typedPtr}
{}

template<typename T>
	util::TWeakSharedHandle<T>::TWeakSharedHandle(const std::shared_ptr<PtrSharedHandleData> &data,T *typedDataPtr)
		: m_data{data},m_typedPtr{typedDataPtr}
{}
template<typename T>
	util::TWeakSharedHandle<T> util::TWeakSharedHandle<T>::operator=(const TWeakSharedHandle<T> &other)
{
	m_data = other.m_data;
	m_typedPtr = other.m_typedPtr;
	return *this;
}
template<typename T>
	util::TWeakSharedHandle<T> util::TWeakSharedHandle<T>::operator=(const TSharedHandle<T> &other)
{
	m_data = other.m_data;
	m_typedPtr = other.m_typedPtr;
	return *this;
}
template<typename T>
	T *util::TWeakSharedHandle<T>::operator->() {return Get();}
template<typename T>
	const T *util::TWeakSharedHandle<T>::operator->() const {return const_cast<util::TWeakSharedHandle<T>*>(this)->operator->();}
template<typename T>
	T &util::TWeakSharedHandle<T>::operator*() {return *Get();}
template<typename T>
	const T &util::TWeakSharedHandle<T>::operator*() const {return const_cast<util::TWeakSharedHandle<T>*>(this)->operator*();}
template<typename T>
	bool util::TWeakSharedHandle<T>::IsValid() const {return m_data.expired() == false && m_data.lock()->GetData() != nullptr;}
template<typename T>
bool util::TWeakSharedHandle<T>::IsExpired() const {return IsValid() == false;}
template<typename T>
	const T *util::TWeakSharedHandle<T>::Get() const {return const_cast<TWeakSharedHandle<T>*>(this)->Get();}
template<typename T>
	T *util::TWeakSharedHandle<T>::Get() {return IsValid() ? m_typedPtr : nullptr;}
template<typename T>
	const T *util::TWeakSharedHandle<T>::GetRawPtr() const {return const_cast<TWeakSharedHandle<T>*>(this)->GetRawPtr();}
template<typename T>
	T *util::TWeakSharedHandle<T>::GetRawPtr() {return m_typedPtr;}
template<typename T>
	void util::TWeakSharedHandle<T>::Remove() {m_data.lock()->Invalidate();}
template<typename T>
	std::shared_ptr<util::PtrSharedHandleData> util::TWeakSharedHandle<T>::GetInternalData() const {return m_data.lock();}

//

template<class TSrc,class TDst>
	util::TWeakSharedHandle<TDst> util::weak_shared_handle_cast(const TWeakSharedHandle<TSrc> &handle)
{
	if constexpr (std::is_convertible<TSrc*,TDst*>::value)
		return util::TWeakSharedHandle<TDst>{handle.GetInternalData(),static_cast<TDst*>(const_cast<TSrc*>(handle.Get()))};
	else
		return util::TWeakSharedHandle<TDst>{handle.GetInternalData(),dynamic_cast<TDst*>(const_cast<TSrc*>(handle.Get()))};
}

#endif
