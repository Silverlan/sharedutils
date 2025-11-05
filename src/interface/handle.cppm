// SPDX-FileCopyrightText: (c) 2025 Silverlan <opensource@pragma-engine.com>
// SPDX-License-Identifier: MIT

module;

#include "definitions.hpp"

export module pragma.util:handle;

export import std.compat;

export {
	namespace util {
		class DLLSHUTIL PtrHandleData {
		public:
			PtrHandleData(void *data);
			void Invalidate();
			void *GetData();
		private:
			void *m_data = nullptr;
		};

		template<typename T>
		class THandle {
		public:
			using value_type = T;
			THandle(const THandle<T> &) = default;
			THandle(const std::shared_ptr<PtrHandleData> &data);
			THandle();
			THandle(std::nullptr_t);
			inline ~THandle() {}
			explicit operator bool() const noexcept { return IsValid(); }
			bool operator!() const noexcept { return !static_cast<bool>(*this); }

			bool operator==(const THandle<T> &other) const;
			bool operator!=(const THandle<T> &other) const { return !operator==(other); }
			bool operator==(const T *other) const;
			bool operator!=(const T *other) const { return !operator==(other); }
			bool operator==(std::nullptr_t) const;
			bool operator!=(std::nullptr_t) const;
			T *operator->();
			const T *operator->() const;
			T &operator*();
			const T &operator*() const;
			bool IsValid() const;
			bool IsExpired() const;
			const T *Get() const;
			T *Get();
			void Remove();
			void Invalidate() { Remove(); }

			// Alias methods for compatibility with shared_ptr
			bool valid() const { return IsValid(); }
			bool expired() const { return IsExpired(); }
			const T *get() const { return Get(); }
			T *get() { return Get(); }

			template<class TT>
			TT *get()
			{
				return static_cast<TT *>(get());
			}
			template<class TT>
			const TT *get() const
			{
				return static_cast<TT *>(get());
			}

			size_t use_count() const { return m_data.use_count(); }
			bool unique() const { return use_count() == 1; }

			const std::shared_ptr<PtrHandleData> &GetInternalData() const;
		private:
			std::shared_ptr<PtrHandleData> m_data = nullptr;
		};

		template<typename T>
		THandle<T>::THandle() : m_data {nullptr}
		{
		}

		template<typename T>
		THandle<T>::THandle(std::nullptr_t) : m_data {nullptr}
		{
		}

		template<typename T>
		THandle<T>::THandle(const std::shared_ptr<PtrHandleData> &data) : m_data {data}
		{
		}
		template<typename T>
		bool THandle<T>::operator==(std::nullptr_t) const
		{
			return !operator!=(nullptr);
		}
		template<typename T>
		bool THandle<T>::operator!=(std::nullptr_t) const
		{
			return IsValid();
		}
		template<typename T>
		bool THandle<T>::operator==(const THandle<T> &other) const {
			if (!m_data)
				return !other.m_data;
			return m_data->GetData() == other.m_data->GetData();
		}
		template<typename T>
		bool THandle<T>::operator==(const T *other) const {
			if (!m_data)
				return !other;
			return m_data->GetData() == other;
		}
		template<typename T>
		T *THandle<T>::operator->()
		{
			return Get();
		}
		template<typename T>
		const T *THandle<T>::operator->() const
		{
			return const_cast<THandle<T> *>(this)->operator->();
		}
		template<typename T>
		T &THandle<T>::operator*()
		{
			return *Get();
		}
		template<typename T>
		const T &THandle<T>::operator*() const
		{
			return const_cast<THandle<T> *>(this)->operator*();
		}
		template<typename T>
		bool THandle<T>::IsValid() const
		{
			return m_data && m_data->GetData() != nullptr;
		}
		template<typename T>
		bool THandle<T>::IsExpired() const
		{
			return IsValid() == false;
		}
		template<typename T>
		const T *THandle<T>::Get() const
		{
			return const_cast<THandle<T> *>(this)->Get();
		}
		template<typename T>
		T *THandle<T>::Get()
		{
			return IsValid() ? static_cast<T*>(m_data->GetData()) : nullptr;
		}
		template<typename T>
		void THandle<T>::Remove()
		{
			m_data->Invalidate();
		}

		template<typename T>
		const std::shared_ptr<PtrHandleData> &THandle<T>::GetInternalData() const
		{
			return m_data;
		}

		//

		template<class TDst>
		THandle<TDst> create_handle(TDst *ptr)
		{
			return THandle<TDst> {std::make_shared<PtrHandleData>(ptr)};
		}
	};
}
