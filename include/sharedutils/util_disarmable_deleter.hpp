/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef __UTIL_SHARED_RELEASE_DELETER_HPP__
#define __UTIL_SHARED_RELEASE_DELETER_HPP__

#include <memory>

namespace util
{
	template<typename T,typename Deleter=typename std::default_delete<T>>
		struct DisarmableDeleter :
			private Deleter
	{
	public:
		static T *release(std::shared_ptr<T> &ptr)
		{
			auto *deleter = std::get_deleter<util::DisarmableDeleter<T>>(ptr);
			deleter->release();
			return ptr.get();
		}
		void operator()(T* ptr)
		{
			if(m_bArmed)
				Deleter::operator()(ptr);
		}
		void release()
		{
			m_bArmed = false;
		}
	private:
		bool m_bArmed = true;
	};
};

#endif
