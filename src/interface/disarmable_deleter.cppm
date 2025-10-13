// SPDX-FileCopyrightText: (c) 2019 Silverlan <opensource@pragma-engine.com>
// SPDX-License-Identifier: MIT

module;

#include <memory>

export module pragma.util:disarmable_deleter;

export namespace util {
	template<typename T, typename Deleter = typename std::default_delete<T>>
	struct DisarmableDeleter : private Deleter {
	  public:
		static T *release(std::shared_ptr<T> &ptr)
		{
			auto *deleter = std::get_deleter<util::DisarmableDeleter<T>>(ptr);
			deleter->release();
			return ptr.get();
		}
		void operator()(T *ptr)
		{
			if(m_bArmed)
				Deleter::operator()(ptr);
		}
		void release() { m_bArmed = false; }
	  private:
		bool m_bArmed = true;
	};
};
