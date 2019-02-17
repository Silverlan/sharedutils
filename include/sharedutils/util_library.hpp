/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef __UTIL_LIBRARY_HPP__
#define __UTIL_LIBRARY_HPP__

#if defined(_WIN32) || defined(__linux__)
#include "utildefinitions.h"
#include <string>
#include <memory>
#include <vector>

#ifdef _WIN32
#include <Windows.h>
#else
#include <dlfcn.h>
#endif

namespace util
{
#ifdef _WIN32
	using LibraryModule = HMODULE;
#else
	using LibraryModule = void*;
#endif
	class DLLSHUTIL Library
	{
	public:
		static std::shared_ptr<Library> Load(const std::string &name,const std::vector<std::string> &additionalSearchDirectories={},std::string *outErr=nullptr);

		void *FindSymbolAddress(const std::string &name) const;
		template<typename TFunc>
			inline TFunc FindSymbolAddress(const std::string &name) const;
		Library(const Library&)=delete;
		Library &operator=(const Library&)=delete;
		~Library();
	private:
		Library(LibraryModule hModule);
		LibraryModule m_module = nullptr;
	};
};

template<typename TFunc>
	TFunc util::Library::FindSymbolAddress(const std::string &name) const
{
	return static_cast<TFunc>(FindSymbolAddress(name));
}

#endif

#endif
