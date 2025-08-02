// SPDX-FileCopyrightText: (c) 2019 Silverlan <opensource@pragma-engine.com>
// SPDX-License-Identifier: MIT

#ifndef __UTIL_LIBRARY_HPP__
#define __UTIL_LIBRARY_HPP__

#if defined(_WIN32) || defined(__linux__)
#include "utildefinitions.h"
#include <string>
#include <memory>
#include <vector>

#ifdef _WIN32
#include <Windows.h>
#endif

namespace util {
#ifdef _WIN32
	using LibraryModule = HMODULE;
#else
	using LibraryModule = void *;
#endif
	class DLLSHUTIL Library {
	  public:
		static std::shared_ptr<Library> Load(const std::string &name, const std::vector<std::string> &additionalSearchDirectories = {}, std::string *outErr = nullptr);
		static std::shared_ptr<Library> Get(const std::string &name, std::string *outErr = nullptr);

		void *FindSymbolAddress(const std::string &name) const;
		template<typename TFunc>
		inline TFunc FindSymbolAddress(const std::string &name) const;
		void SetDontFreeLibraryOnDestruct();
		Library(const Library &) = delete;
		Library &operator=(const Library &) = delete;
		~Library();
	  private:
		Library(const std::string &name, LibraryModule hModule);
		LibraryModule m_module = nullptr;
		bool m_freeOnDestruct = true;
		std::string m_name;
	};
};

template<typename TFunc>
TFunc util::Library::FindSymbolAddress(const std::string &name) const
{
	return reinterpret_cast<TFunc>(FindSymbolAddress(name));
}

#endif

#endif
