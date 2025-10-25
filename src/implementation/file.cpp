// SPDX-FileCopyrightText: (c) 2019 Silverlan <opensource@pragma-engine.com>
// SPDX-License-Identifier: MIT

module;

#include <optional>
#include <string>

#include <functional>

module pragma.util;

import :file;

bool ufile::get_extension(const std::string &f, std::string *ext)
{
	size_t epos = f.rfind(".");
	size_t dpos = f.find_last_of("\\/");
	if(epos != std::string::npos && (epos > dpos + 1 || dpos == std::string::npos)) {
		*ext = f.substr(epos + 1);
		ustring::to_lower(*ext);
		return true;
	}
	return false;
}

bool ufile::compare_extension(const std::string &f, const std::vector<std::string> &extensions)
{
	std::string ext;
	if(!get_extension(f, &ext))
		return false;
	for(int i = 0; i < extensions.size(); i++)
		if(extensions[i] == ext)
			return true;
	return false;
}

bool ufile::compare_directory(const std::string &f, const std::vector<std::string> &folders)
{
	for(int i = 0; i < folders.size(); i++) {
		size_t l = folders[i].length();
		if(f.substr(0, l) == folders[i] && f[l] == '\\')
			return true;
	}
	return false;
}

std::string ufile::get_path_from_filename(const std::string &str)
{
	size_t br = str.find_last_of("/\\");
	if(br == ustring::NOT_FOUND)
		return "";
	return str.substr(0, br + 1);
}

std::string ufile::get_file_from_filename(const std::string &str)
{
	size_t br = str.find_last_of("/\\");
	if(br == ustring::NOT_FOUND)
		return str;
	return str.substr(br + 1);
}

std::optional<std::string> ufile::remove_extension_from_filename(std::string &str)
{
	std::string ext;
	if(get_extension(str, &ext) == true) {
		str = str.substr(0, str.length() - (ext.length() + 1));
		return ext;
	}
	return {};
}

std::vector<std::string> ufile::split_path(const std::string &path)
{
	auto npath = path;
	ustring::replace(npath, "/", "\\");
	std::vector<std::string> r;
	ustring::explode(path, "\\", r);
	return r;
}
void ufile::to_lower(std::string &str) { return ustring::to_lower(str); }

std::string ufile::to_path(const std::vector<std::string> &splitPath, size_t startPos, size_t endPos)
{
	if(splitPath.empty())
		return "";
	std::string r {};
	if(endPos == std::string::npos)
		endPos = splitPath.size() - 1ull;
	for(auto i = startPos; i <= endPos; ++i) {
		if(i > startPos)
			r += '/';
		r += splitPath.at(i);
	}
	return r;
}
