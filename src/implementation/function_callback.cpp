// SPDX-FileCopyrightText: (c) 2019 Silverlan <opensource@pragma-engine.com>
// SPDX-License-Identifier: MIT

module;




module pragma.util;

import :function_callback;

#ifdef CALLBACK_SANITY_CHECK_ENABLED
TCallback::TCallback(size_t hashCode) : hashCode(hashCode) {}
#else
TCallback::TCallback() {}
#endif
TCallback::TCallback(const TCallback &other)
#ifdef CALLBACK_SANITY_CHECK_ENABLED
	: hashCode {other.hashCode}
#endif
{}
TCallback::~TCallback() {}

TCallback &TCallback::operator=(const TCallback &other)
{
#ifdef CALLBACK_SANITY_CHECK_ENABLED
	hashCode = other.hashCode;
#endif
	return *this;
}
bool TCallback::operator==(void *) const { return false; }
bool TCallback::operator!=(void *) const { return true; }
bool TCallback::operator==(const TCallback &other) const { return false; }
bool TCallback::operator!=(const TCallback &other) const { return true; }
void TCallback::operator()() {}
void TCallback::Release() { delete this; }

/////////////////////////////

CallbackHandle::CallbackHandle() : util::BaseCallbackHandle {} {}
CallbackHandle::CallbackHandle(const CallbackHandle &other)
	: util::BaseCallbackHandle{other}
{}
CallbackHandle::CallbackHandle(const std::shared_ptr<TCallback> &t) : util::BaseCallbackHandle {t} {}
CallbackHandle::~CallbackHandle() {}
CallbackHandle &CallbackHandle::operator=(const CallbackHandle &other)
{
	util::BaseCallbackHandle::operator=(other);
	return *this;
}
void CallbackHandle::operator()()
{
	if(!IsValid())
		return;
	auto cb = get();
	(*cb)();
}

void CallbackHandle::Remove()
{
	if(!IsValid())
		return;
	Invalidate();
}

/////////////////////////////

util::BaseCallbackHandle::BaseCallbackHandle(TPtr *t) : m_basePointer(t), m_bEmpty(false)
{
}
util::BaseCallbackHandle::BaseCallbackHandle(const TPtr &t) : BaseCallbackHandle(new TPtr(t))
{
}
util::BaseCallbackHandle::BaseCallbackHandle() : m_basePointer(nullptr), m_bEmpty(true)
{
}
util::BaseCallbackHandle::~BaseCallbackHandle() {}

bool util::BaseCallbackHandle::IsValid() const
{
	return (!m_bEmpty && get() != nullptr) ? true : false;
}

util::BaseCallbackHandle::TUnderlyingType *util::BaseCallbackHandle::get() const
{
	if(m_bEmpty || m_basePointer.get() == nullptr)
		return nullptr;
	auto &target = *m_basePointer.get();
	return (target != nullptr) ? &*target : nullptr;
}

void util::BaseCallbackHandle::Invalidate()
{
	*m_basePointer = nullptr;
}

int32_t util::BaseCallbackHandle::use_count() const
{
	return m_basePointer.use_count();
}

bool util::BaseCallbackHandle::unique() const
{
	return m_basePointer.use_count() == 1;
}

bool util::BaseCallbackHandle::operator==(const TPtr &other) const
{
	if(other == nullptr)
		return !IsValid();
	return (*m_basePointer.get() == other) ? true : false;
}

bool util::BaseCallbackHandle::operator!=(const TPtr &other) const
{
	if(other == nullptr)
		return IsValid();
	return (*m_basePointer.get() != other) ? true : false;
}

bool util::BaseCallbackHandle::operator==(const util::BaseCallbackHandle &other) const
{
	if(other == nullptr)
		return !IsValid();
	return (get() == other.get()) ? true : false;
}

bool util::BaseCallbackHandle::operator!=(const util::BaseCallbackHandle &other) const
{
	if(other == nullptr)
		return IsValid();
	return (get() != other.get()) ? true : false;
}

bool util::BaseCallbackHandle::operator==(const std::nullptr_t) const
{
	return !IsValid();
}

bool util::BaseCallbackHandle::operator!=(const std::nullptr_t) const
{
	return IsValid();
}

util::BaseCallbackHandle::TUnderlyingType *util::BaseCallbackHandle::operator->()
{
	return get();
}
