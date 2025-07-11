// SPDX-FileCopyrightText: (c) 2019 Silverlan <opensource@pragma-engine.com>
// SPDX-License-Identifier: MIT

#ifndef __UNIFIEDPTR_H__
#define __UNIFIEDPTR_H__

#include "utildefinitions.h"
#include <memory>

template<class T>
class UnifiedPtr;
class DLLSHUTIL UnifiedBasePtr {
  protected:
	UnifiedBasePtr() = default;
  public:
	virtual ~UnifiedBasePtr() = default;
	template<class T>
	T *GetData() const;
	template<class T>
	T GetValue() const;
};

template<class T>
T *UnifiedBasePtr::GetData() const
{
	if(typeid(*this) != typeid(UnifiedPtr<T>))
		return nullptr;
	return static_cast<const UnifiedPtr<T> *const>(this)->get();
}

template<class T>
T UnifiedBasePtr::GetValue() const
{
	if(typeid(*this) != typeid(UnifiedPtr<T>))
		return T();
	return *static_cast<const UnifiedPtr<T> *const>(this)->get();
}

template<class T>
class UnifiedPtr : public UnifiedBasePtr, public std::unique_ptr<T> {
  public:
	UnifiedPtr(const T &t);
	UnifiedPtr(T *t);
};

template<class T>
UnifiedPtr<T>::UnifiedPtr(T *t) : UnifiedBasePtr(), std::unique_ptr<T>(t)
{
}

template<class T>
UnifiedPtr<T>::UnifiedPtr(const T &t) : UnifiedPtr<T>(new T(t))
{
}

#endif
