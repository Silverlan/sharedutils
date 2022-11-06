/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "sharedutils/util.h"
#include <cstring>
#if defined(_WIN32) || defined(__linux__)
#include "sharedutils/util_library.hpp"
#include "sharedutils/util_string.h"
#include "sharedutils/util_file.h"
#include "sharedutils/scope_guard.h"
#ifdef __linux__
#include <dlfcn.h>
#include <ltdl.h>
#endif



using namespace util;
std::shared_ptr<Library> Library::Get(const std::string &name,std::string *outErr)
{
#ifdef _WIN32
	auto nName = name;
	ufile::remove_extension_from_filename(nName);
	nName += ".dll";
	ustring::replace(nName,"/","\\");
	auto hModule = GetModuleHandleA(nName.c_str());
	if(hModule == nullptr)
		return nullptr;
#else
//auto hModule = lt_dlopen(name.c_str(),RTLD_LAZY | RTLD_GLOBAL);
    lt_dlinit();
    lt_dladvise libSettings;
    lt_dladvise_init(&libSettings);
    lt_dladvise_global(&libSettings);
    auto hModule = lt_dlopenadvise(name.c_str(),libSettings);
    lt_dladvise_destroy(&libSettings);
    if(hModule == nullptr)
    {
        if(outErr != nullptr)
            *outErr = lt_dlerror();
        return nullptr;
	}
#endif
	auto lib = std::shared_ptr<Library>(new Library(hModule));
	lib->m_freeOnDestruct = false;
	return lib;
}
std::shared_ptr<Library> Library::Load(const std::string &name,const std::vector<std::string> &additionalSearchDirectories,std::string *outErr)
{
#ifdef _WIN32
	std::vector<DLL_DIRECTORY_COOKIE> dirCookies;
	dirCookies.reserve(additionalSearchDirectories.size());
	for(auto &searchPath : additionalSearchDirectories)
	{
		auto nSearchPath = searchPath;
		ustring::replace(nSearchPath,"/","\\");
		auto cookie = AddDllDirectory(ustring::string_to_wstring(nSearchPath).c_str());
		if(cookie == nullptr)
			continue;
		dirCookies.push_back(cookie);
	}
	auto dllGuard = ScopeGuard([dirCookies]() {
		for(auto &cookie : dirCookies)
			RemoveDllDirectory(cookie);
	});
	auto nName = name;
	ufile::remove_extension_from_filename(nName);
	nName += ".dll";
	ustring::replace(nName,"/","\\");
	auto hModule = LoadLibraryEx(nName.c_str(),nullptr,LOAD_LIBRARY_SEARCH_DLL_LOAD_DIR | LOAD_LIBRARY_SEARCH_DEFAULT_DIRS);
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
    lt_dlinit();
    const char* curLibPathRaw = lt_dlgetsearchpath();
    std::string curLibPath = curLibPathRaw!=nullptr ? curLibPathRaw:"";
    std::string newLibPath;
    if(curLibPath.size()!=0)
    {
        newLibPath = curLibPath+":";
    }
    newLibPath = newLibPath + ustring::implode(additionalSearchDirectories,":"); //Those are loaded LAST.

    lt_dlsetsearchpath(newLibPath.c_str());


    util::ScopeGuard sgResetLibPath {[curLibPath=std::move(curLibPath)]() {
        if(curLibPath.size()!=0)
               lt_dlsetsearchpath(curLibPath.c_str());
    }};

    std::string soName = name;
    ufile::remove_extension_from_filename(soName,std::vector<std::string>{"so"});
    soName += ".so";

    //auto hModule = lt_dlopen(soName.c_str(),RTLD_LAZY | RTLD_GLOBAL);
    //lt_dlinit();
    lt_dladvise libSettings;
    lt_dladvise_init(&libSettings);
    lt_dladvise_global(&libSettings);
    auto hModule = lt_dlopenadvise(soName.c_str(),libSettings);
    lt_dladvise_destroy(&libSettings);
    if(hModule == nullptr)
    {
        if(outErr != nullptr)
            *outErr = lt_dlerror();
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
	if(m_freeOnDestruct == false)
#ifdef __linux__
        lt_dlexit();
#endif
		return;
#ifdef _WIN32
	FreeLibrary(m_module);
#else
    lt_dlclose(m_module);
    lt_dlexit();
#endif
}

void Library::SetDontFreeLibraryOnDestruct() {
 #ifdef __linux__
    const lt_dlinfo* libData = lt_dlgetinfo(m_module); //This is a copy not a ref to module data.
    std::string libPath = libData->filename;
    lt_dladvise libSettings;
    lt_dladvise_init(&libSettings);
    lt_dladvise_global(&libSettings);
    lt_dladvise_resident(&libSettings);
    m_module = lt_dlopenadvise(libPath.c_str(),libSettings);
    lt_dladvise_destroy(&libSettings);

#if 0

    const lt_dlinfo* libData_postResidence = lt_dlgetinfo(m_module); //This is a copy not a ref to module data.
#endif
#endif
    m_freeOnDestruct = false;
}

void *Library::FindSymbolAddress(const std::string &name) const
{
#ifdef _WIN32
	return GetProcAddress(m_module,name.c_str());
#else
    return lt_dlsym(m_module,name.c_str());
#endif
}

#endif
