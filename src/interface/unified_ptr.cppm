// SPDX-FileCopyrightText: (c) 2019 Silverlan <opensource@pragma-engine.com>
// SPDX-License-Identifier: MIT

export module pragma.util:unified_ptr;

export import std.compat;

export {
	template<class T>
	class UnifiedPtr;
	class DLLSHUTIL UnifiedBasePtr {
	  protected:
		UnifiedBasePtr() = default;
	  public:
		virtual ~UnifiedBasePtr() = default;
		template<class T>
		T *GetData() const
		{
			if(typeid(*this) != typeid(UnifiedPtr<T>))
				return nullptr;
			return static_cast<const UnifiedPtr<T> *const>(this)->get();
		}
		template<class T>
		T GetValue() const
		{
			if(typeid(*this) != typeid(UnifiedPtr<T>))
				return T();
			return *static_cast<const UnifiedPtr<T> *const>(this)->get();
		}
	};

	template<class T>
	class UnifiedPtr : public UnifiedBasePtr, public std::unique_ptr<T> {
	  public:
		UnifiedPtr(const T &t) : UnifiedBasePtr(), std::unique_ptr<T>(t) {}
		UnifiedPtr(T *t) : UnifiedPtr<T>(new T(t)) {}
	};
}
