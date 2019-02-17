/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef __CALLBACK_HANDLER_H__
#define __CALLBACK_HANDLER_H__

#include "utildefinitions.h"
#include <unordered_map>
#include <string>
#include <vector>
#include <queue>
#include <memory>
#include "sharedutils/functioncallback.h"
#include "sharedutils/util_string.h"

#pragma warning(push)
#pragma warning(disable : 4251)
template<typename T>
	class TCallbackHandler
{
public:
	TCallbackHandler();
	virtual ~TCallbackHandler();
	CallbackHandle AddCallback(const T &identifier,const CallbackHandle &hCallback);
	std::vector<CallbackHandle> *GetCallbacks(const T &identifier);
	template<class TRet,typename... TARGS>
		void RegisterCallback(const T &identifier);
	template<class TRet,typename... TARGS>
		void RegisterCallbackWithOptionalReturn(const T &identifier);

	void CallCallbacks(const T &identifier);

	template<typename... TARGS>
		void CallCallbacks(const T &identifier,TARGS ...args);
	template<class TRet,typename... TARGS>
		TRet CallCallbacks(const T &identifier,TARGS ...args);
	// Avoid using this overload; Use CallCallbacksWithOptionalReturn instead
	template<class TRet,typename... TARGS>
		CallbackReturnType CallCallbacks(const T &identifier,TRet *ret,TARGS ...args);
	template<class TRet,typename... TARGS>
		CallbackReturnType CallCallbacksWithOptionalReturn(const T &identifier,TRet &ret,TARGS ...args);
protected:
	struct CallbackInfo
	{
		CallbackInfo();
		uint32_t callDepth;
		std::vector<CallbackHandle> callbacks;
#ifdef CALLBACK_SANITY_CHECK_ENABLED
		size_t hashCode = std::numeric_limits<size_t>::max();
#endif
	};
	std::unordered_map<T,CallbackInfo> m_callbacks;
	virtual T TranslateIdentifier(const T &identifier) const;
private:
	std::shared_ptr<bool> m_bAlive;
	struct StackInfo
	{
		StackInfo(const T &n,const CallbackHandle &hCb);
		T name;
		CallbackHandle hCallback;
	};
	bool m_bValid;
	std::vector<StackInfo> m_callbackQueue;
	void ProcessCallbackStack(const T &identifier);
};

class DLLSHUTIL CallbackHandler
	: public TCallbackHandler<std::string>
{
protected:
	virtual std::string TranslateIdentifier(const std::string &identifier) const override;
};
#pragma warning(pop)

#include "callback_handler_impl.h"

#endif
