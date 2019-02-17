/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef __UTIL_NULL_REF_HPP__
#define __UTIL_NULL_REF_HPP__

namespace util
{
	// TODO: This is bad design and should be avoided. If C++17 is available, use std::optional instead.
	static const void *_snptr = nullptr;
	template<typename T>
		T &nullref() {return *static_cast<T*>(const_cast<void*>(_snptr));}
	template<typename T>
		const T &const_nullref() {return *static_cast<const T*>(_snptr);}

	template<typename T>
		T &optional_ref() {return nullref<T>();}
	template<typename T>
		const T &optional_const_ref() {return const_nullref<T>();}

	template<typename T>
		bool is_ref_set(const T &t) {return &t != nullptr;}
};

#endif
