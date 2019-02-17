/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "sharedutils/util_link.hpp"

#ifdef _WIN32
#include <windows.h>
#include <winnls.h>
#include <shobjidl.h>
#include <objbase.h>
#include <objidl.h>
#include <shlguid.h>
#include <strsafe.h>
#include <array>
#include <filesystem>
#include "sharedutils/scope_guard.h"

// Source: https://docs.microsoft.com/de-de/windows/desktop/shell/links
static HRESULT create_link(LPCSTR lpszPathObj, LPCSTR lpszPathLink, LPCSTR lpszDesc) 
{ 
    HRESULT hres; 
    IShellLink* psl; 

    // Get a pointer to the IShellLink interface. It is assumed that CoInitialize
    // has already been called.
    hres = CoCreateInstance(CLSID_ShellLink, NULL, CLSCTX_INPROC_SERVER, IID_IShellLink, (LPVOID*)&psl); 
    if (SUCCEEDED(hres)) 
    { 
        IPersistFile* ppf; 

        // Set the path to the shortcut target and add the description. 
        psl->SetPath(lpszPathObj); 
        psl->SetDescription(lpszDesc); 

        // Query IShellLink for the IPersistFile interface, used for saving the 
        // shortcut in persistent storage. 
        hres = psl->QueryInterface(IID_IPersistFile, (LPVOID*)&ppf); 

        if (SUCCEEDED(hres)) 
        { 
            WCHAR wsz[MAX_PATH]; 

            // Ensure that the string is Unicode. 
            MultiByteToWideChar(CP_ACP, 0, lpszPathLink, -1, wsz, MAX_PATH); 

            // Save the link by calling IPersistFile::Save. 
            hres = ppf->Save(wsz, TRUE); 
            ppf->Release(); 
        } 
        psl->Release(); 
    } 
    return hres; 
}

static HRESULT resolve_link(HWND hwnd, LPCSTR lpszLinkFile, LPSTR lpszPath, int iPathBufferSize) 
{ 
    HRESULT hres; 
    IShellLink* psl; 
    CHAR szGotPath[MAX_PATH]; 
    CHAR szDescription[MAX_PATH]; 
    WIN32_FIND_DATA wfd; 
 
    *lpszPath = 0; // Assume failure 

    // Get a pointer to the IShellLink interface. It is assumed that CoInitialize
    // has already been called. 
    hres = CoCreateInstance(CLSID_ShellLink, NULL, CLSCTX_INPROC_SERVER, IID_IShellLink, (LPVOID*)&psl); 
    if (SUCCEEDED(hres)) 
    { 
        IPersistFile* ppf; 
 
        // Get a pointer to the IPersistFile interface. 
        hres = psl->QueryInterface(IID_IPersistFile, (void**)&ppf); 
        
        if (SUCCEEDED(hres)) 
        { 
            WCHAR wsz[MAX_PATH]; 
 
            // Ensure that the string is Unicode. 
            MultiByteToWideChar(CP_ACP, 0, lpszLinkFile, -1, wsz, MAX_PATH); 
 
            // Add code here to check return value from MultiByteWideChar 
            // for success.
 
            // Load the shortcut. 
            hres = ppf->Load(wsz, STGM_READ); 
            
            if (SUCCEEDED(hres)) 
            { 
                // Resolve the link. 
                hres = psl->Resolve(hwnd, 0); 

                if (SUCCEEDED(hres)) 
                { 
                    // Get the path to the link target. 
                    hres = psl->GetPath(szGotPath, MAX_PATH, (WIN32_FIND_DATA*)&wfd, SLGP_SHORTPATH); 

                    if (SUCCEEDED(hres)) 
                    { 
                        // Get the description of the target. 
                        hres = psl->GetDescription(szDescription, MAX_PATH); 

                        if (SUCCEEDED(hres)) 
                        {
                            hres = StringCbCopy(lpszPath, iPathBufferSize, szGotPath);
                            if (SUCCEEDED(hres))
                            {
                                // Handle success
                            }
                            else
                            {
                                // Handle the error
                            }
                        }
                    }
                } 
            } 

            // Release the pointer to the IPersistFile interface. 
            ppf->Release(); 
        } 

        // Release the pointer to the IShellLink interface. 
        psl->Release(); 
    } 
    return hres; 
}

static void initialize_co()
{
	static auto initialized = false;
	if(initialized == true)
		return;
	initialized = true;
	if(CoInitialize(NULL))
	{
		// Cleanup at program shutdown
		static ScopeGuard sg {[]() {
			CoUninitialize();
		}};
	}
}

bool util::create_link(const std::string &srcPath,const std::string &lnkPath)
{
	initialize_co();
	auto fullLnkPath = lnkPath +".lnk";
	return SUCCEEDED(::create_link(srcPath.c_str(),fullLnkPath.c_str(),""));
}
bool util::resolve_link(const std::string &lnkPath,std::string &outResolvedPath)
{
	initialize_co();
	auto fullLnkPath = lnkPath +".lnk";
	std::array<CHAR,MAX_PATH> resolvedPath;
	auto r = ::resolve_link(NULL,fullLnkPath.c_str(),resolvedPath.data(),resolvedPath.size());
	if(SUCCEEDED(r) == false)
		return false;
	outResolvedPath = resolvedPath.data();
	return true;
}
bool util::link_exists(const std::string &lnkPath)
{
	auto fullLnkPath = lnkPath +".lnk";
	return std::filesystem::exists(fullLnkPath);
}
#else
#include <unistd.h>
#include <filesystem>

bool util::create_link(const std::string &srcPath,const std::string &lnkPath)
{
	return symlink(srcPath.c_str(),lnkPath.c_str()) == 0;
}
bool util::resolve_link(const std::string &lnkPath,std::string &outResolvedPath)
{
	auto *resolvedPath = realpath(lnkPath.c_str(),nullptr);
	if(resolvedPath == nullptr)
		return false;
	outResolvedPath = resolvedPath;
	free(resolvedPath);
	return true;
}
bool util::link_exists(const std::string &lnkPath)
{
	return std::filesystem::exists(lnkPath);
}
#endif
