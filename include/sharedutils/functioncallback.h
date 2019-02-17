/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef __FUNCTIONCALLBACK_H__
#define __FUNCTIONCALLBACK_H__

#include "utildefinitions.h"
#include "def_handle.h"
#include <memory>
#include <functional>

#define CALLBACK_SANITY_CHECK_ENABLED

enum class CallbackReturnType : uint8_t
{
	NoReturnValue = 0u,
	HasReturnValue
};

class CallbackHandle;
class DLLSHUTIL TCallback
{
protected:
	void Release();
public:
#ifdef CALLBACK_SANITY_CHECK_ENABLED
	TCallback(size_t hashCode) : hashCode(hashCode) {}
#else
	TCallback()=default;
#endif
	virtual ~TCallback()=default;
	virtual bool operator==(void *p);
	virtual bool operator!=(void *p);
	virtual void operator()();
#ifdef CALLBACK_SANITY_CHECK_ENABLED
	size_t hashCode;
#endif
};

template<typename T,typename... TARGS>
	class Callback
		: public TCallback
{
protected:
	std::function<T(TARGS...)> m_function;
public:
	Callback(const std::function<T(TARGS...)> &function)
		: 
#ifdef CALLBACK_SANITY_CHECK_ENABLED
		TCallback(typeid(std::function<T(TARGS...)>).hash_code()),
#else
		TCallback(),
#endif
		m_function(function)
	{}
	void SetFunction(const std::function<T(TARGS...)> &f) {m_function = f;}
	T operator()(TARGS ...args) {return m_function(std::forward<TARGS>(args)...);}
};

class DLLSHUTIL CallbackHandle
	: public util::BaseHandle<std::shared_ptr<TCallback>,TCallback>
{
public:
	using util::BaseHandle<std::shared_ptr<TCallback>,TCallback>::BaseHandle;
	void Remove();
	void operator()();
	template<typename... TARGS>
		void operator()(TARGS ...args);
	template<typename T COMMA typename... TARGS>
		T Call(TARGS ...args);
	template<typename T COMMA typename... TARGS>
		CallbackReturnType Call(T *ret,TARGS ...args);
};

template<typename... TARGS>
	void CallbackHandle::operator()(TARGS ...args)
{
	Call<void,TARGS...>(std::forward<TARGS>(args)...);
}

template<typename T,typename... TARGS>
	T CallbackHandle::Call(TARGS ...args)
{
	if(!IsValid())
		return T();
	auto cb = get<Callback<T,TARGS...>>();
#ifdef CALLBACK_SANITY_CHECK_ENABLED
	if(cb == nullptr && get()->hashCode != typeid(std::function<void(void)>).hash_code())
		throw std::invalid_argument("Attempted to call callback with template arguments that are incompatible with specified template arguments!");
#endif
	if(cb != nullptr)
		return (*cb)(std::forward<TARGS>(args)...);
	else
		(*this)();
	return T();
}

template<typename T COMMA typename... TARGS>
	CallbackReturnType CallbackHandle::Call(T *ret,TARGS ...args)
{
	if(!IsValid())
		return CallbackReturnType::NoReturnValue;
	auto cb = get<Callback<CallbackReturnType,T*,TARGS...>>();
#ifdef CALLBACK_SANITY_CHECK_ENABLED
	if(cb == nullptr && get()->hashCode != typeid(std::function<void(void)>).hash_code())
		throw std::invalid_argument("Attempted to call callback with template arguments that are incompatible with specified template arguments!");
#endif
	if(cb != nullptr)
	{
		if((*cb)(ret,std::forward<TARGS>(args)...) == CallbackReturnType::HasReturnValue)
			return CallbackReturnType::HasReturnValue;
	}
	else
		(*this)();
	return CallbackReturnType::NoReturnValue;
}

////////////////////////////////////////////////////////////////////////////////////

template<typename T=void,typename... TARGS>
	class FunctionCallback
{
private:
	FunctionCallback()=delete;
public:
	static CallbackHandle Create(const std::function<T(TARGS...)> &function);
	static CallbackHandle CreateWithOptionalReturn(const std::function<CallbackReturnType(T*,TARGS...)> &function);
};

template<typename T,typename... TARGS>
	CallbackHandle FunctionCallback<T,TARGS...>::Create(const std::function<T(TARGS...)> &function)
{
	return CallbackHandle(std::shared_ptr<Callback<T,TARGS...>>(new Callback<T,TARGS...>(function)));
}

template<typename T,typename... TARGS>
	CallbackHandle FunctionCallback<T,TARGS...>::CreateWithOptionalReturn(const std::function<CallbackReturnType(T*,TARGS...)> &function)
{
	return FunctionCallback<CallbackReturnType,T*,TARGS...>::Create(function);
}

#endif
