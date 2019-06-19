/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef __CALLBACK_HANDLER_IMPL_H__
#define __CALLBACK_HANDLER_IMPL_H__

#include <stdexcept>

template<typename T>
	TCallbackHandler<T>::CallbackInfo::CallbackInfo()
		: callDepth(0)
{}

template<typename T>
	TCallbackHandler<T>::StackInfo::StackInfo(const T &n,const CallbackHandle &hCb)
		: name(n),hCallback(hCb)
{}

template<typename T>
	TCallbackHandler<T>::TCallbackHandler()
		: m_bAlive(std::make_shared<bool>(true))
{}

template<typename T>
	TCallbackHandler<T>::~TCallbackHandler()
{
	m_bAlive = nullptr;
}

template<typename T>
	T TCallbackHandler<T>::TranslateIdentifier(const T &identifier) const
{
	return identifier;
}

template<typename T>
	void TCallbackHandler<T>::ProcessCallbackStack(const T &identifier)
{
	for(auto it=m_callbackQueue.begin();it!=m_callbackQueue.end();)
	{
		auto &info = *it;
		if(info.name == identifier)
		{
			AddCallback(info.name,info.hCallback);
			it = m_callbackQueue.erase(it);
		}
		else
			++it;
	}
}

template<typename T>
	CallbackHandle TCallbackHandler<T>::AddCallback(const T &pidentifier,const CallbackHandle &hCallback)
{
	auto identifier = TranslateIdentifier(pidentifier);
	auto it = m_callbacks.find(identifier);
	if(it == m_callbacks.end())
		it = m_callbacks.insert(typename decltype(m_callbacks)::value_type(identifier,{})).first;
#ifdef CALLBACK_SANITY_CHECK_ENABLED
	if(hCallback.get()->hashCode != typeid(std::function<void(void)>).hash_code() && it->second.hashCode != hCallback.get()->hashCode)
		throw std::invalid_argument("Attempted to add callback with template arguments that are incompatible with registered callback!");
#endif
	else if(it->second.callDepth > 0)
	{
		m_callbackQueue.push_back({identifier,hCallback});
		return m_callbackQueue.back().hCallback;
	}
	it->second.callbacks.push_back(hCallback);
	return it->second.callbacks.back();
}

template<typename T>
	std::vector<CallbackHandle> *TCallbackHandler<T>::GetCallbacks(const T &pidentifier)
{
	auto identifier = TranslateIdentifier(pidentifier);
	auto it = m_callbacks.find(identifier);
	if(it == m_callbacks.end())
		return nullptr;
	return &it->second.callbacks;
}

template<typename T>
	void TCallbackHandler<T>::CallCallbacks(const T &pidentifier)
{
	auto identifier = TranslateIdentifier(pidentifier);
	auto it = m_callbacks.find(identifier);
	if(it == m_callbacks.end())
		return;
#ifdef CALLBACK_SANITY_CHECK_ENABLED
	if(it->second.hashCode != typeid(std::function<void(void)>).hash_code())
		throw std::invalid_argument("CallCallbacks template arguments differ from arguments that were used when the callback was registered!");
#endif
	auto bAlive = std::weak_ptr<bool>(m_bAlive); // We need to keep this reference, in case 'this' was invalidated during a callback
	auto &callbackInfo = it->second;
	++callbackInfo.callDepth;
	auto &callbacks = callbackInfo.callbacks;
	for(auto itCb=callbacks.begin();bAlive.expired() == false && itCb!=callbacks.end();)
	{
		auto &hCallback = *itCb;
		if(hCallback.IsValid())
		{
#ifdef CALLBACK_SANITY_CHECK_ENABLED
			if(hCallback.get()->hashCode != typeid(std::function<void(void)>).hash_code() && hCallback.get()->hashCode != it->second.hashCode)
				throw std::invalid_argument("Callback template arguments differ from arguments that were used when the callback was registered!");
#endif
			++itCb;
			hCallback();
		}
		else
			itCb = callbacks.erase(itCb);
	}
	if(bAlive.expired() == true)
		return;
	if(--callbackInfo.callDepth == 0)
		ProcessCallbackStack(identifier);
#ifndef CALLBACK_SANITY_CHECK_ENABLED
	if(callbacks.empty())
		m_callbacks.erase(it);
#endif
}

template<typename T>
	template<typename... TARGS>
		void TCallbackHandler<T>::CallCallbacks(const T &pidentifier,TARGS ...args)
{
	auto identifier = TranslateIdentifier(pidentifier);
	auto it = m_callbacks.find(identifier);
	if(it == m_callbacks.end())
		return;
#ifdef CALLBACK_SANITY_CHECK_ENABLED
	if(it->second.hashCode != typeid(std::function<void(TARGS...)>).hash_code())
		throw std::invalid_argument("CallCallbacks template arguments differ from arguments that were used when the callback was registered!");
#endif
	auto bAlive = std::weak_ptr<bool>(m_bAlive); // We need to keep this reference, in case 'this' was invalidated during a callback
	auto &callbackInfo = it->second;
	++callbackInfo.callDepth;
	auto &callbacks = callbackInfo.callbacks;
	for(auto itCb=callbacks.begin();bAlive.expired() == false && itCb!=callbacks.end();)
	{
		auto &hCallback = *itCb;
		if(hCallback.IsValid())
		{
#ifdef CALLBACK_SANITY_CHECK_ENABLED
			if(hCallback.get()->hashCode != typeid(std::function<void(void)>).hash_code() && hCallback.get()->hashCode != it->second.hashCode)
				throw std::invalid_argument("Callback template arguments differ from arguments that were used when the callback was registered!");
#endif
			++itCb;
			hCallback(std::forward<TARGS>(args)...);
		}
		else
			itCb = callbacks.erase(itCb);
	}
	if(bAlive.expired() == true)
		return;
	if(--callbackInfo.callDepth == 0)
		ProcessCallbackStack(identifier);
#ifndef CALLBACK_SANITY_CHECK_ENABLED
	if(callbacks.empty())
		m_callbacks.erase(it);
#endif
}

template<typename T>
	template<class TRet,typename... TARGS>
		TRet TCallbackHandler<T>::CallCallbacks(const T &pidentifier,TARGS ...args)
{
	auto identifier = TranslateIdentifier(pidentifier);
	auto it = m_callbacks.find(identifier);
	if(it == m_callbacks.end())
		return TRet();
#ifdef CALLBACK_SANITY_CHECK_ENABLED
	if(it->second.hashCode != typeid(std::function<TRet(TARGS...)>).hash_code())
		throw std::invalid_argument("CallCallbacks template arguments differ from arguments that were used when the callback was registered!");
#endif
	auto bAlive = std::weak_ptr<bool>(m_bAlive); // We need to keep this reference, in case 'this' was invalidated during a callback
	auto &callbackInfo = it->second;
	++callbackInfo.callDepth;
	auto &callbacks = callbackInfo.callbacks;
	for(auto itCb=callbacks.begin();bAlive.expired() == false && itCb!=callbacks.end();)
	{
		auto &hCallback = *itCb;
		if(hCallback.IsValid())
		{
#ifdef CALLBACK_SANITY_CHECK_ENABLED
			if(hCallback.get()->hashCode != typeid(std::function<void(void)>).hash_code() && hCallback.get()->hashCode != it->second.hashCode)
				throw std::invalid_argument("Callback template arguments differ from arguments that were used when the callback was registered!");
#endif
			++itCb;
			if(!std::is_void<TRet>())
				return hCallback.template Call<TRet>(std::forward<TARGS>(args)...);
			else
				hCallback.template Call<TRet>(std::forward<TARGS>(args)...);
		}
		else
			itCb = callbacks.erase(itCb);
	}
	if(bAlive.expired() == true)
		return TRet();
	if(--callbackInfo.callDepth == 0)
		ProcessCallbackStack(identifier);
#ifndef CALLBACK_SANITY_CHECK_ENABLED
	if(callbacks.empty())
		m_callbacks.erase(it);
#endif
	return TRet();
}

template<typename T>
	template<class TRet,typename... TARGS>
		CallbackReturnType TCallbackHandler<T>::CallCallbacks(const T &pidentifier,TRet *ret,TARGS ...args)
{
	auto identifier = TranslateIdentifier(pidentifier);
	auto it = m_callbacks.find(identifier);
	if(it == m_callbacks.end())
		return CallbackReturnType::NoReturnValue;
#ifdef CALLBACK_SANITY_CHECK_ENABLED
	if(it->second.hashCode != typeid(std::function<CallbackReturnType(TRet*,TARGS...)>).hash_code())
		throw std::invalid_argument("CallCallbacks template arguments differ from arguments that were used when the callback was registered!");
#endif
	auto bAlive = std::weak_ptr<bool>(m_bAlive); // We need to keep this reference, in case 'this' was invalidated during a callback
	auto &callbackInfo = it->second;
	++callbackInfo.callDepth;
	auto &callbacks = callbackInfo.callbacks;
	for(auto itCb=callbacks.begin();bAlive.expired() == false && itCb!=callbacks.end();)
	{
		auto &hCallback = *itCb;
		if(hCallback.IsValid())
		{
#ifdef CALLBACK_SANITY_CHECK_ENABLED
			if(hCallback.get()->hashCode != typeid(std::function<void(void)>).hash_code() && hCallback.get()->hashCode != it->second.hashCode)
				throw std::invalid_argument("Callback template arguments differ from arguments that were used when the callback was registered!");
#endif
			++itCb;
			if(hCallback.template Call<TRet>(ret,std::forward<TARGS>(args)...) == CallbackReturnType::HasReturnValue)
				return CallbackReturnType::HasReturnValue;
		}
		else
			itCb = callbacks.erase(itCb);
	}
	if(bAlive.expired() == true)
		return CallbackReturnType::NoReturnValue;
	if(--callbackInfo.callDepth == 0)
		ProcessCallbackStack(identifier);
#ifndef CALLBACK_SANITY_CHECK_ENABLED
	if(callbacks.empty())
		m_callbacks.erase(it);
#endif
	return CallbackReturnType::NoReturnValue;
}

template<typename T>
	template<class TRet,typename... TARGS>
		CallbackReturnType TCallbackHandler<T>::CallCallbacksWithOptionalReturn(const T &identifier,TRet &ret,TARGS ...args)
{
	return CallCallbacks<TRet,TARGS...>(identifier,&ret,std::forward<TARGS>(args)...);
}

template<typename T>
	template<class TRet,typename... TARGS>
		void TCallbackHandler<T>::RegisterCallback(const T &pidentifier)
{
#ifdef CALLBACK_SANITY_CHECK_ENABLED
	auto identifier = TranslateIdentifier(pidentifier);
	auto it = m_callbacks.find(identifier);
	if(it == m_callbacks.end())
		it = m_callbacks.insert(std::make_pair(identifier,CallbackInfo{})).first;
	it->second.hashCode = typeid(std::function<TRet(TARGS...)>).hash_code();
#endif
}

template<typename T>
	template<class TRet,typename... TARGS>
		void TCallbackHandler<T>::RegisterCallbackWithOptionalReturn(const T &pidentifier)
{
	if(!std::is_void<TRet>())
		RegisterCallback<CallbackReturnType,TRet*,TARGS...>(pidentifier);
	else
		RegisterCallback<void,TARGS...>(pidentifier);
}

#endif
