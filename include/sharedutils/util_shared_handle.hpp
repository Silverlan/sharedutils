/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef __UTIL_SHARED_HANDLE_HPP__
#define __UTIL_SHARED_HANDLE_HPP__

#include "utildefinitions.h"
#include <memory>

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
		class TSharedHandle
	{
	public:
		TSharedHandle(const TSharedHandle<T>&)=default;
		TSharedHandle(const std::shared_ptr<PtrSharedHandleData> &data);
		TSharedHandle();
		TSharedHandle(nullptr_t);
		TSharedHandle(T *data);
		T *operator->();
		const T *operator->() const;
		T &operator*();
		const T &operator*() const;
		bool IsValid() const;
		bool IsExpired() const;
		const T *Get() const;
		T *Get();
		void Remove();

		const std::shared_ptr<PtrSharedHandleData> &GetInternalData() const;
	private:
		std::shared_ptr<PtrSharedHandleData> m_data = nullptr;
	};

	template<typename T>
		class TWeakSharedHandle
	{
	public:
		TWeakSharedHandle(const TWeakSharedHandle<T>&)=default;
		TWeakSharedHandle(const TSharedHandle<T> &sharedHandle);
		TWeakSharedHandle();
		operator TSharedHandle<T>() const;
		
		T *operator->();
		const T *operator->() const;
		T &operator*();
		const T &operator*() const;
		bool IsValid() const;
		bool IsExpired() const;
		const T *Get() const;
		T *Get();
		void Remove();
	private:
		std::weak_ptr<PtrSharedHandleData> m_data = nullptr;
	};
};

template<typename T>
	util::TSharedHandle<T>::TSharedHandle()
		: m_data{nullptr}
{}

template<typename T>
	util::TSharedHandle<T>::TSharedHandle(nullptr_t)
		: m_data{nullptr}
{}

template<typename T>
	util::TSharedHandle<T>::TSharedHandle(T *data)
		: m_data{std::make_shared<PtrSharedHandleData>(std::shared_ptr<T>{data})}
{}

template<typename T>
	util::TSharedHandle<T>::TSharedHandle(const std::shared_ptr<PtrSharedHandleData> &data)
		: m_data{data}
{}
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
	T *util::TSharedHandle<T>::Get() {return m_data ? static_cast<T*>(m_data->GetData()) : nullptr;}
template<typename T>
	void util::TSharedHandle<T>::Remove() {m_data->Invalidate();}
	
template<typename T>
	const std::shared_ptr<util::PtrSharedHandleData> &util::TSharedHandle<T>::GetInternalData() const {return m_data;}

//////////////////////
	
template<typename T>
	util::TWeakSharedHandle<T>::TWeakSharedHandle()
		: m_data{}
{}

template<typename T>
	util::TWeakSharedHandle<T>::TWeakSharedHandle(const TSharedHandle<T> &sharedHandle)
		: m_data{sharedHandle.GetInternalData()}
{}
	
template<typename T>
	util::TWeakSharedHandle<T>::operator util::TSharedHandle<T>() const {return TSharedHandle<T>{m_data.lock()};}
	
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
	T *util::TWeakSharedHandle<T>::Get() {return m_data.expired() == false ? static_cast<T*>(m_data.lock()->GetData()) : nullptr;}
template<typename T>
	void util::TWeakSharedHandle<T>::Remove() {m_data.lock()->Invalidate();}

#endif
