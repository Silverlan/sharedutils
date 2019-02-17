/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef __UTIL_FILE_H__
#define __UTIL_FILE_H__

#include "utildefinitions.h"
#include <string>
#include <vector>
namespace ufile
{
	DLLSHUTIL bool get_extension(const std::string &f,std::string *ext);
	DLLSHUTIL bool compare_extension(const std::string &f,std::vector<std::string> *extensions);
	DLLSHUTIL bool compare_directory(const std::string &f,std::vector<std::string> *folders);
	DLLSHUTIL std::string get_path_from_filename(const std::string &str);
	DLLSHUTIL std::string get_file_from_filename(const std::string &str);
	DLLSHUTIL void remove_extension_from_filename(std::string &str);
	DLLSHUTIL std::vector<std::string> split_path(const std::string &path);
	DLLSHUTIL std::string to_path(const std::vector<std::string> &splitPath,size_t startPos=0ull,size_t endPos=std::string::npos);
}

#endif