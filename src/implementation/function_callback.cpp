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
{
}
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

CallbackHandle::CallbackHandle() : pragma::util::BaseCallbackHandle {} {}
CallbackHandle::CallbackHandle(const CallbackHandle &other) : pragma::util::BaseCallbackHandle {other} {}
CallbackHandle::CallbackHandle(const std::shared_ptr<TCallback> &t) : pragma::util::BaseCallbackHandle {t} {}
CallbackHandle::~CallbackHandle() {}
CallbackHandle &CallbackHandle::operator=(const CallbackHandle &other)
{
	BaseCallbackHandle::operator=(other);
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

pragma::util::BaseCallbackHandle::BaseCallbackHandle(TPtr *t) : m_basePointer(t), m_bEmpty(false) {}
pragma::util::BaseCallbackHandle::BaseCallbackHandle(const TPtr &t) : BaseCallbackHandle(new TPtr(t)) {}
pragma::util::BaseCallbackHandle::BaseCallbackHandle() : m_basePointer(nullptr), m_bEmpty(true) {}
pragma::util::BaseCallbackHandle::~BaseCallbackHandle() {}

bool pragma::util::BaseCallbackHandle::IsValid() const { return (!m_bEmpty && get() != nullptr) ? true : false; }

pragma::util::BaseCallbackHandle::TUnderlyingType *pragma::util::BaseCallbackHandle::get() const
{
	if(m_bEmpty || m_basePointer.get() == nullptr)
		return nullptr;
	auto &target = *m_basePointer.get();
	return (target != nullptr) ? &*target : nullptr;
}

void pragma::util::BaseCallbackHandle::Invalidate() { *m_basePointer = nullptr; }

int32_t pragma::util::BaseCallbackHandle::use_count() const { return m_basePointer.use_count(); }

bool pragma::util::BaseCallbackHandle::unique() const { return m_basePointer.use_count() == 1; }

bool pragma::util::BaseCallbackHandle::operator==(const TPtr &other) const
{
	if(other == nullptr)
		return !IsValid();
	return (*m_basePointer.get() == other) ? true : false;
}

bool pragma::util::BaseCallbackHandle::operator!=(const TPtr &other) const
{
	if(other == nullptr)
		return IsValid();
	return (*m_basePointer.get() != other) ? true : false;
}

bool pragma::util::BaseCallbackHandle::operator==(const BaseCallbackHandle &other) const
{
	if(other == nullptr)
		return !IsValid();
	return (get() == other.get()) ? true : false;
}

bool pragma::util::BaseCallbackHandle::operator!=(const BaseCallbackHandle &other) const
{
	if(other == nullptr)
		return IsValid();
	return (get() != other.get()) ? true : false;
}

bool pragma::util::BaseCallbackHandle::operator==(const std::nullptr_t) const { return !IsValid(); }

bool pragma::util::BaseCallbackHandle::operator!=(const std::nullptr_t) const { return IsValid(); }

pragma::util::BaseCallbackHandle::TUnderlyingType *pragma::util::BaseCallbackHandle::operator->() { return get(); }
