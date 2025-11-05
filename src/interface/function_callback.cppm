// SPDX-FileCopyrightText: (c) 2019 Silverlan <opensource@pragma-engine.com>
// SPDX-License-Identifier: MIT

module;

#include "definitions.hpp"

export module pragma.util:function_callback;

export import std.compat;

#define COMMA ,

// #define CALLBACK_SANITY_CHECK_ENABLED

export {
	enum class CallbackReturnType : uint8_t { NoReturnValue = 0u, HasReturnValue };

	class CallbackHandle;
	class DLLSHUTIL TCallback {
	protected:
		void Release();
	public:
	#ifdef CALLBACK_SANITY_CHECK_ENABLED
		TCallback(size_t hashCode);
	#else
		TCallback();
	#endif
		TCallback(const TCallback &other);
		virtual ~TCallback();
		TCallback &operator=(const TCallback &other);
		virtual bool operator==(void *p) const;
		virtual bool operator!=(void *p) const;
		virtual bool operator==(const TCallback &other) const;
		virtual bool operator!=(const TCallback &other) const;
		virtual void operator()();
	#ifdef CALLBACK_SANITY_CHECK_ENABLED
		size_t hashCode;
	#endif
	};

	template<typename T, typename... TARGS>
	class Callback : public TCallback {
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
		{
		}
		void SetFunction(const std::function<T(TARGS...)> &f) { m_function = f; }
		T operator()(TARGS... args) { return m_function(std::forward<TARGS>(args)...); }
	};

	namespace util {
		class DLLSHUTIL BaseCallbackHandle {
			using TPtr = std::shared_ptr<TCallback>;
			using TUnderlyingType = TCallback;
		public:
			BaseCallbackHandle();
			BaseCallbackHandle(TPtr *t);
			BaseCallbackHandle(const TPtr &t);
			virtual ~BaseCallbackHandle();
			bool IsValid() const;
			bool operator==(const TPtr &other) const;
			bool operator!=(const TPtr &other) const;
			bool operator==(const BaseCallbackHandle &other) const;
			bool operator!=(const BaseCallbackHandle &other) const;
			bool operator==(const std::nullptr_t other) const;
			bool operator!=(const std::nullptr_t other) const;
			void Invalidate();
			TUnderlyingType *operator->();
			TUnderlyingType *get() const;
			template<class T>
			T *get() const;
			int32_t use_count() const;
			bool unique() const;
		protected:
			std::shared_ptr<TPtr> m_basePointer;
			bool m_bEmpty;
		};

		template<class T>
		T *BaseCallbackHandle::get() const {
			return dynamic_cast<T *>(get());
		}
	};

	class DLLSHUTIL CallbackHandle : public util::BaseCallbackHandle {
	public:
		CallbackHandle();
		CallbackHandle(const CallbackHandle &other);
		CallbackHandle(const std::shared_ptr<TCallback> &t);
		~CallbackHandle() override;
		void Remove();
		CallbackHandle &operator=(const CallbackHandle &other);
		void operator()();
		template<typename... TARGS>
		void operator()(TARGS... args)
		{
			Call<void, TARGS...>(std::forward<TARGS>(args)...);
		}
		template<typename T COMMA typename... TARGS>
		T Call(TARGS... args)
		{
			if(!IsValid())
				return T();
			auto cb = get<Callback<T, TARGS...>>();
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
		CallbackReturnType Call(T *ret, TARGS... args)
		{
			if(!IsValid())
				return CallbackReturnType::NoReturnValue;
			auto cb = get<Callback<CallbackReturnType, T *, TARGS...>>();
		#ifdef CALLBACK_SANITY_CHECK_ENABLED
			if(cb == nullptr && get()->hashCode != typeid(std::function<void(void)>).hash_code())
				throw std::invalid_argument("Attempted to call callback with template arguments that are incompatible with specified template arguments!");
		#endif
			if(cb != nullptr) {
				if((*cb)(ret, std::forward<TARGS>(args)...) == CallbackReturnType::HasReturnValue)
					return CallbackReturnType::HasReturnValue;
			}
			else
				(*this)();
			return CallbackReturnType::NoReturnValue;
		}
	};

	////////////////////////////////////////////////////////////////////////////////////

	template<typename T = void, typename... TARGS>
	class FunctionCallback {
	private:
		FunctionCallback() = delete;
	public:
		static CallbackHandle Create(const std::function<T(TARGS...)> &function)
		{
			return CallbackHandle(std::shared_ptr<Callback<T, TARGS...>>(new Callback<T, TARGS...>(function)));
		}
		static CallbackHandle CreateWithOptionalReturn(const std::function<CallbackReturnType(T *, TARGS...)> &function)
		{
			return FunctionCallback<CallbackReturnType, T *, TARGS...>::Create(function);
		}
	};
}
