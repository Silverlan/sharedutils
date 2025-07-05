// SPDX-FileCopyrightText: (c) 2019 Silverlan <opensource@pragma-engine.com>
// SPDX-License-Identifier: MIT

#ifndef __UTIL_NULL_REF_HPP__
#define __UTIL_NULL_REF_HPP__

namespace util {
	// TODO: This is bad design and should be avoided. If C++17 is available, use std::optional instead.
	static const void *_snptr = nullptr;
	template<typename T>
	T &nullref()
	{
		return *static_cast<T *>(const_cast<void *>(_snptr));
	}
	template<typename T>
	const T &const_nullref()
	{
		return *static_cast<const T *>(_snptr);
	}

	template<typename T>
	T &optional_ref()
	{
		return nullref<T>();
	}
	template<typename T>
	const T &optional_const_ref()
	{
		return const_nullref<T>();
	}

	template<typename T>
	bool is_ref_set(const T &t)
	{
		return &t != nullptr;
	}
};

#endif
