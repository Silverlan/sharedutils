// SPDX-FileCopyrightText: (c) 2019 Silverlan <opensource@pragma-engine.com>
// SPDX-License-Identifier: MIT

#ifndef __UTIL_FILE_H__
#define __UTIL_FILE_H__

#include "utildefinitions.h"
#include <string>
#include <vector>
#include <optional>
#include <algorithm>

namespace ufile {
	DLLSHUTIL bool get_extension(const std::string &f, std::string *ext);
	DLLSHUTIL bool compare_extension(const std::string &f, const std::vector<std::string> &extensions);
	DLLSHUTIL bool compare_directory(const std::string &f, const std::vector<std::string> &folders);
	DLLSHUTIL std::string get_path_from_filename(const std::string &str);
	DLLSHUTIL std::string get_file_from_filename(const std::string &str);
	DLLSHUTIL std::optional<std::string> remove_extension_from_filename(std::string &str);
	DLLSHUTIL std::vector<std::string> split_path(const std::string &path);
	DLLSHUTIL std::string to_path(const std::vector<std::string> &splitPath, size_t startPos = 0ull, size_t endPos = std::string::npos);
	DLLSHUTIL void to_lower(std::string &str);
	template<typename TList>
	std::optional<std::string> remove_extension_from_filename(std::string &str, const TList &extensions)
	{
		std::string ext;
		if(get_extension(str, &ext) == false)
			return {};
		to_lower(ext);
		auto it = std::find(extensions.begin(), extensions.end(), ext);
		if(it == extensions.end())
			return {};
		str = str.substr(0, str.length() - (ext.length() + 1));
		return ext;
	}
	template<typename TList>
	std::optional<std::string> get_file_extension(const std::string &str, const TList &extensions)
	{
		std::string ext;
		if(get_extension(str, &ext) == false)
			return {};
		to_lower(ext);
		auto it = std::find(extensions.begin(), extensions.end(), ext);
		if(it == extensions.end())
			return {};
		return ext;
	}
}

#endif
