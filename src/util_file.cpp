/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "sharedutils/util_file.h"
#include "sharedutils/util_string.h"
#include "sharedutils/property/util_property.hpp"

bool ufile::get_extension(const std::string &f,std::string *ext)
{
	size_t epos = f.rfind(".");
	size_t dpos = f.find_last_of("\\/");
	if(epos != std::string::npos && (epos > dpos +1 || dpos == std::string::npos))
	{
		*ext = f.substr(epos +1);
		return true;
	}
	return false;
}

bool ufile::compare_extension(const std::string &f,std::vector<std::string> *extensions)
{
	std::string ext;
	if(!get_extension(f,&ext))
		return false;
	for(int i=0;i<extensions->size();i++)
		if((*extensions)[i] == ext)
			return true;
	return false;
}

bool ufile::compare_directory(const std::string &f,std::vector<std::string> *folders)
{
	for(int i=0;i<folders->size();i++)
	{
		size_t l = (*folders)[i].length();
		if(f.substr(0,l) == (*folders)[i] && f[l] == '\\')
			return true;
	}
	return false;
}

std::string ufile::get_path_from_filename(const std::string &str)
{
	size_t br = str.find_last_of("/\\");
	if(br == ustring::NOT_FOUND) return "";
	return str.substr(0,br +1);
}

std::string ufile::get_file_from_filename(const std::string &str)
{
	size_t br = str.find_last_of("/\\");
	if(br == ustring::NOT_FOUND) return str;
	return str.substr(br +1);
}

void ufile::remove_extension_from_filename(std::string &str)
{
	std::string ext;
	if(get_extension(str,&ext) == true)
		str = str.substr(0,str.length() -(ext.length() +1));
}

std::vector<std::string> ufile::split_path(const std::string &path)
{
	auto npath = path;
	ustring::replace(npath,"/","\\");
	std::vector<std::string> r;
	ustring::explode(path,"\\",r);
	return r;
}

std::string ufile::to_path(const std::vector<std::string> &splitPath,size_t startPos,size_t endPos)
{
	if(splitPath.empty())
		return "";
	std::string r {};
	if(endPos == std::string::npos)
		endPos = splitPath.size() -1ull;
	for(auto i=startPos;i<=endPos;++i)
	{
		if(i > startPos)
			r += '/';
		r += splitPath.at(i);
	}
	return r;
}
