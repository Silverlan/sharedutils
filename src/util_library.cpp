/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#if defined(_WIN32) || defined(__linux__)
#include "sharedutils/util_library.hpp"
#include "sharedutils/util_string.h"
#include "sharedutils/scope_guard.h"

using namespace util;
std::shared_ptr<Library> Library::Load(const std::string &name,const std::vector<std::string> &additionalSearchDirectories,std::string *outErr)
{
#ifdef _WIN32
	std::vector<DLL_DIRECTORY_COOKIE> dirCookies;
	dirCookies.reserve(additionalSearchDirectories.size());
	for(auto &searchPath : additionalSearchDirectories)
	{
		auto cookie = AddDllDirectory(ustring::string_to_wstring(searchPath).c_str());
		if(cookie == nullptr)
			continue;
		dirCookies.push_back(cookie);
	}
	auto dllGuard = ScopeGuard([dirCookies]() {
		for(auto &cookie : dirCookies)
			RemoveDllDirectory(cookie);
	});
	auto hModule = LoadLibraryEx(name.c_str(),nullptr,LOAD_LIBRARY_SEARCH_DLL_LOAD_DIR | LOAD_LIBRARY_SEARCH_DEFAULT_DIRS);
	if(hModule == nullptr)
	{
		if(outErr != nullptr)
		{
			auto errCode = GetLastError();
			if(errCode != 0)
			{
				LPTSTR lpErrorText = NULL;
				auto size = FormatMessageA(
					FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
					NULL,errCode,MAKELANGID(LANG_NEUTRAL,SUBLANG_DEFAULT),(LPSTR)&lpErrorText,0,NULL);
				*outErr = std::string(lpErrorText,size);
				::LocalFree(lpErrorText);
			}
			else
				*outErr = "Unknown error.";
		}
		return nullptr;
	}
#else
	// TODO: Take additionalSearchDirectories into account
	auto hModule = dlopen(name.c_str(),RTLD_LAZY | RTLD_GLOBAL);
	if(hModule == nullptr)
	{
		if(outErr != nullptr)
			*outErr = dlerror();
		return nullptr;
	}
#endif
	return std::shared_ptr<Library>(new Library(hModule));
}

Library::Library(LibraryModule hModule)
	: m_module{hModule}
{}

Library::~Library()
{
#ifdef _WIN32
	FreeLibrary(m_module);
#else
	dlclose(m_module);
#endif
}

void *Library::FindSymbolAddress(const std::string &name) const
{
#ifdef _WIN32
	return GetProcAddress(m_module,name.c_str());
#else
	return dlsym(m_module,name.c_str());
#endif
}

#endif
