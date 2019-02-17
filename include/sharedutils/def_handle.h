/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef __DEF_HANDLE_H__
#define __DEF_HANDLE_H__

#include <cstddef>
#include <memory>

namespace util
{
	namespace _impl
	{
		template<class T,class TFriend>
			struct PtrTarget
		{
		public:
			friend TFriend;
			PtrTarget(const T &t);
		protected:
			T target;
			void Invalidate();
		};
	};
	template<class TBaseClass,class TInternalClass=TBaseClass>
		class BaseHandle
			: private std::shared_ptr<_impl::PtrTarget<TBaseClass,BaseHandle<TBaseClass,TInternalClass>>>
	{
	protected:
		bool m_bEmpty;
		using std::shared_ptr<_impl::PtrTarget<TBaseClass,BaseHandle<TBaseClass,TInternalClass>>>::reset;
	public:
		BaseHandle();
		BaseHandle(_impl::PtrTarget<TBaseClass,BaseHandle<TBaseClass,TInternalClass>> *t);
		BaseHandle(const TBaseClass &t);
		virtual ~BaseHandle()=default;
		bool IsValid() const;
		bool operator==(const TBaseClass &other) const;
		bool operator!=(const TBaseClass &other) const;
		bool operator==(const BaseHandle &other) const;
		bool operator!=(const BaseHandle &other) const;
		bool operator==(const std::nullptr_t other) const;
		bool operator!=(const std::nullptr_t other) const;
		void Invalidate();
		TInternalClass *operator->();
		TInternalClass *get() const;
		template<class T>
			T *get() const;
		int32_t use_count() const;
		bool unique() const;
	};
};

template<class T,class TFriend>
	util::_impl::PtrTarget<T,TFriend>::PtrTarget(const T &t)
	: target(t)
{}
template<class T,class TFriend>
	void util::_impl::PtrTarget<T,TFriend>::Invalidate() {target = T();}

template<class TBaseClass,class TInternalClass>
	util::BaseHandle<TBaseClass,TInternalClass>::BaseHandle(_impl::PtrTarget<TBaseClass,BaseHandle<TBaseClass,TInternalClass>> *t)
		: std::shared_ptr<_impl::PtrTarget<TBaseClass,BaseHandle<TBaseClass,TInternalClass>>>(t),m_bEmpty(false)
{}
template<class TBaseClass,class TInternalClass>
	util::BaseHandle<TBaseClass,TInternalClass>::BaseHandle(const TBaseClass &t)
		: BaseHandle<TBaseClass,TInternalClass>(new _impl::PtrTarget<TBaseClass,BaseHandle<TBaseClass,TInternalClass>>(t))
{}
template<class TBaseClass,class TInternalClass>
	util::BaseHandle<TBaseClass,TInternalClass>::BaseHandle::BaseHandle()
		: std::shared_ptr<_impl::PtrTarget<TBaseClass,BaseHandle<TBaseClass,TInternalClass>>>(nullptr),m_bEmpty(true)
{}

template<class TBaseClass,class TInternalClass>
	bool util::BaseHandle<TBaseClass,TInternalClass>::IsValid() const
{return (!m_bEmpty && get() != nullptr && std::shared_ptr<_impl::PtrTarget<TBaseClass,BaseHandle<TBaseClass,TInternalClass>>>::get()->target != nullptr) ? true : false;}

template<class TBaseClass,class TInternalClass>
	TInternalClass *util::BaseHandle<TBaseClass,TInternalClass>::get() const {if(m_bEmpty || std::shared_ptr<_impl::PtrTarget<TBaseClass,BaseHandle<TBaseClass,TInternalClass>>>::get() == nullptr) return nullptr; return &*std::shared_ptr<_impl::PtrTarget<TBaseClass,BaseHandle<TBaseClass,TInternalClass>>>::get()->target;}

template<class TBaseClass,class TInternalClass>
	void util::BaseHandle<TBaseClass,TInternalClass>::Invalidate() {std::shared_ptr<_impl::PtrTarget<TBaseClass,BaseHandle<TBaseClass,TInternalClass>>>::get()->Invalidate();}

template<class TBaseClass,class TInternalClass>
	int32_t util::BaseHandle<TBaseClass,TInternalClass>::use_count() const {return std::shared_ptr<_impl::PtrTarget<TBaseClass,BaseHandle<TBaseClass,TInternalClass>>>::use_count();}

template<class TBaseClass,class TInternalClass>
	bool util::BaseHandle<TBaseClass,TInternalClass>::unique() const {return std::shared_ptr<_impl::PtrTarget<TBaseClass,BaseHandle<TBaseClass,TInternalClass>>>::unique();}

template<class TBaseClass,class TInternalClass>
	bool util::BaseHandle<TBaseClass,TInternalClass>::operator==(const TBaseClass &other) const
{
	if(other == nullptr)
		return !IsValid();
	return (std::shared_ptr<_impl::PtrTarget<TBaseClass,BaseHandle<TBaseClass,TInternalClass>>>::get()->target == other) ? true : false;
}

template<class TBaseClass,class TInternalClass>
	bool util::BaseHandle<TBaseClass,TInternalClass>::operator!=(const TBaseClass &other) const
{
	if(other == nullptr)
		return IsValid();
	return (std::shared_ptr<_impl::PtrTarget<TBaseClass,BaseHandle<TBaseClass,TInternalClass>>>::get()->target != other) ? true : false;
}

template<class TBaseClass,class TInternalClass>
	bool util::BaseHandle<TBaseClass,TInternalClass>::operator==(const util::BaseHandle<TBaseClass,TInternalClass> &other) const
{
	if(other == nullptr)
		return !IsValid();
	return (get() == other.get()) ? true : false;
}

template<class TBaseClass,class TInternalClass>
	bool util::BaseHandle<TBaseClass,TInternalClass>::operator!=(const util::BaseHandle<TBaseClass,TInternalClass> &other) const
{
	if(other == nullptr)
		return IsValid();
	return (get() != other.get()) ? true : false;
}

template<class TBaseClass,class TInternalClass>
	bool util::BaseHandle<TBaseClass,TInternalClass>::operator==(const std::nullptr_t) const
{
	return !IsValid();
}

template<class TBaseClass,class TInternalClass>
	bool util::BaseHandle<TBaseClass,TInternalClass>::operator!=(const std::nullptr_t) const
{
	return IsValid();
}

template<class TBaseClass,class TInternalClass>
	TInternalClass *util::BaseHandle<TBaseClass,TInternalClass>::operator->() {return get();}

template<class TBaseClass,class TInternalClass>
	template<class T>
		T *util::BaseHandle<TBaseClass,TInternalClass>::get() const
{
	return dynamic_cast<T*>(get());
}


// The following are deprecated and should be avoided!

#define COMMA ,

#define DECLARE_BASE_HANDLE_EXT(classdef,classname,localname,extended) \
	struct Ptr##localname; \
	class classdef localname##Handle \
		: std::shared_ptr<Ptr##localname> \
	{ \
	protected: \
		bool m_bEmpty; \
		using std::shared_ptr<Ptr##localname>::reset; \
	public: \
		friend classname; \
	public: \
		localname##Handle(); \
		localname##Handle(Ptr##localname *t); \
		localname##Handle(classname *t); \
		virtual ~localname##Handle(); \
		bool IsValid() const; \
		bool operator==(const classname *other) const; \
		bool operator!=(const classname *other) const; \
		bool operator==(const localname##Handle *other) const; \
		bool operator!=(const localname##Handle *other) const; \
                bool operator==(const std::nullptr_t other) const; \
                bool operator!=(const std::nullptr_t other) const; \
		void Invalidate(); \
		classname *operator->(); \
		classname *get() const; \
		template<class T> \
			T *get() const; \
		localname##Handle *Copy(); \
		int use_count(); \
		bool unique(); \
		extended \
	}; \
	template<class T> \
		T *localname##Handle::get() const \
	{ \
		return dynamic_cast<T*>(get()); \
	} \
	struct classdef Ptr##localname \
	{ \
	public: \
		friend classname; \
		friend localname##Handle; \
		Ptr##localname(classname *t); \
	protected: \
		classname *target; \
		void Invalidate(); \
	};

#define DECLARE_BASE_HANDLE(classdef,classname,localname) \
	DECLARE_BASE_HANDLE_EXT(classdef,classname,localname,);

#define DECLARE_DERIVED_CHILD_HANDLE_EXT(classdef,basehandleidentifier,baseclass,parentclass,classname,localname,extended,vrt) \
	class baseclass; \
	struct Ptr##basehandleidentifier; \
	class classdef localname##Handle \
		: public vrt parentclass##Handle \
	{ \
	public: \
		friend classname; \
		localname##Handle(); \
		localname##Handle(const localname##Handle &t)=default; \
		localname##Handle(const basehandleidentifier##Handle &t); \
		localname##Handle(parentclass##Handle &h); \
		localname##Handle(Ptr##basehandleidentifier *t); \
		localname##Handle(baseclass *t); \
		extended \
	};

#define DECLARE_VIRTUAL_DERIVED_CHILD_HANDLE_EXT(classdef,basehandleidentifier,baseclass,parentclass,classname,localname,extended) \
	DECLARE_DERIVED_CHILD_HANDLE_EXT(classdef,basehandleidentifier,baseclass,parentclass,classname,localname,extended,virtual)

#define DECLARE_DERIVED_CHILD_HANDLE(classdef,basehandleidentifier,baseclass,parentclass,classname,localname) \
	DECLARE_DERIVED_CHILD_HANDLE_EXT(classdef,basehandleidentifier,baseclass,parentclass,classname,localname,,);

#define DECLARE_VIRTUAL_DERIVED_CHILD_HANDLE(classdef,basehandleidentifier,baseclass,parentclass,classname,localname) \
	DECLARE_DERIVED_CHILD_HANDLE_EXT(classdef,basehandleidentifier,baseclass,parentclass,classname,localname,,virtual);

#define DECLARE_DERIVED_HANDLE_EXT(classdef,basehandleidentifier,baseclass,classname,localname,extended) \
	DECLARE_DERIVED_CHILD_HANDLE_EXT(classdef,basehandleidentifier,baseclass,baseclass,classname,localname,extended,);

#define DECLARE_VIRTUAL_DERIVED_HANDLE_EXT(classdef,basehandleidentifier,baseclass,classname,localname,extended) \
	DECLARE_DERIVED_CHILD_HANDLE_EXT(classdef,basehandleidentifier,baseclass,baseclass,classname,localname,extended,virtual);

#define DECLARE_DERIVED_HANDLE(classdef,basehandleidentifier,baseclass,classname,localname) \
	DECLARE_DERIVED_CHILD_HANDLE(classdef,basehandleidentifier,baseclass,baseclass,classname,localname);

#define DEFINE_BASE_HANDLE(classdef,classname,localname) \
	Ptr##localname::Ptr##localname(classname *t) \
		: target(t) \
	{} \
	void Ptr##localname::Invalidate() {target = NULL;} \
	localname##Handle::localname##Handle(Ptr##localname *t) \
		: std::shared_ptr<Ptr##localname>(t),m_bEmpty(false) \
	{} \
	localname##Handle::localname##Handle(classname *t) \
		: localname##Handle(new Ptr##localname(t)) \
	{} \
	localname##Handle::localname##Handle() \
		: std::shared_ptr<Ptr##localname>(nullptr),m_bEmpty(true) \
	{} \
	localname##Handle::~localname##Handle() {} \
	bool localname##Handle::IsValid() const \
	{return (!m_bEmpty && get() != NULL && std::shared_ptr<Ptr##localname>::get()->target != NULL) ? true : false;} \
	classname *localname##Handle::get() const {if(m_bEmpty || std::shared_ptr<Ptr##localname>::get() == NULL) return NULL; return std::shared_ptr<Ptr##localname>::get()->target;} \
	void localname##Handle::Invalidate() {std::shared_ptr<Ptr##localname>::get()->Invalidate();} \
	localname##Handle *localname##Handle::Copy() {return new localname##Handle(*this);} \
	int localname##Handle::use_count() {return std::shared_ptr<Ptr##localname>::use_count();} \
	bool localname##Handle::unique() {return std::shared_ptr<Ptr##localname>::unique();} \
	bool localname##Handle::operator==(const classname *other) const \
	{ \
		if(other == NULL) \
			return !IsValid(); \
		return (std::shared_ptr<Ptr##localname>::get()->target == other) ? true : false; \
	} \
	bool localname##Handle::operator!=(const classname *other) const \
	{ \
		if(other == NULL) \
			return IsValid(); \
		return (std::shared_ptr<Ptr##localname>::get()->target != other) ? true : false; \
	} \
	bool localname##Handle::operator==(const localname##Handle *other) const \
	{ \
		if(other == NULL) \
			return !IsValid(); \
		return (this == other) ? true : false; \
	} \
	bool localname##Handle::operator!=(const localname##Handle *other) const \
	{ \
		if(other == NULL) \
			return IsValid(); \
		return (this != other) ? true : false; \
	} \
	bool localname##Handle::operator==(const std::nullptr_t) const \
	{ \
		return !IsValid(); \
	} \
	bool localname##Handle::operator!=(const std::nullptr_t) const \
	{ \
		return IsValid(); \
	} \
	classname *localname##Handle::operator->() {return get();}

#define DEFINE_DERIVED_CHILD_HANDLE(classdef,basehandleidentifier,baseclass,parentclass,classname,localname) \
	localname##Handle::localname##Handle(const basehandleidentifier##Handle &t) \
		: parentclass##Handle(t) \
	{} \
	localname##Handle::localname##Handle(Ptr##basehandleidentifier *t) \
		: parentclass##Handle(t) \
	{} \
	localname##Handle::localname##Handle(baseclass *t) \
		: localname##Handle(new Ptr##basehandleidentifier(t)) \
	{} \
	localname##Handle::localname##Handle(parentclass##Handle &h) \
		: parentclass##Handle(h) \
	{} \
	localname##Handle::localname##Handle() \
		: parentclass##Handle() \
	{}

#define DEFINE_DERIVED_HANDLE(classdef,basehandleidentifier,baseclass,classname,localname) \
	DEFINE_DERIVED_CHILD_HANDLE(classdef,basehandleidentifier,baseclass,baseclass,classname,localname);

#endif
