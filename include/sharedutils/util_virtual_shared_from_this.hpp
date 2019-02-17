/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef __UTIL_VIRTUAL_SHARED_FROM_THIS_HPP__
#define __UTIL_VIRTUAL_SHARED_FROM_THIS_HPP__

#include "utildefinitions.h"
#include <memory>

// Source: http://stackoverflow.com/a/16083526/2482983
#pragma warning(push)
#pragma warning(disable : 4251)
namespace util
{
	namespace impl
	{
		class DLLSHUTIL MultipleInheritableEnableSharedFromThis
			: public std::enable_shared_from_this<MultipleInheritableEnableSharedFromThis>
		{
		public:
			virtual ~MultipleInheritableEnableSharedFromThis()=default;
		};
	};
	template <class T>
		class inheritable_enable_shared_from_this
			: virtual public impl::MultipleInheritableEnableSharedFromThis
	{
	public:
		std::shared_ptr<T> shared_from_this() {return std::dynamic_pointer_cast<T>(MultipleInheritableEnableSharedFromThis::shared_from_this());}
		/* Utility method to easily downcast.
		* Useful when a child doesn't inherit directly from enable_shared_from_this
		* but wants to use the feature.
		*/
		template <class Down>
			std::shared_ptr<Down> downcasted_shared_from_this() {return std::dynamic_pointer_cast<Down>(MultipleInheritableEnableSharedFromThis::shared_from_this());}
	};
};
#pragma warning(pop)

#endif
