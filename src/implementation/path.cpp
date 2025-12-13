// SPDX-FileCopyrightText: (c) 2020 Silverlan <opensource@pragma-engine.com>
// SPDX-License-Identifier: MIT

module;

#ifdef __linux__
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#else
#include "Shlwapi.h"
#pragma comment(lib, "shlwapi.lib")
#endif

module pragma.util;

import :path;

#undef max

#ifdef _WIN32
static auto USE_CASE_SENSITIVE_PATHS = false;
#else
static auto USE_CASE_SENSITIVE_PATHS = true;
#endif

#undef CreateFile

std::string pragma::util::get_normalized_path(const std::string &path)
{
	auto newPath = path;
	canonicalize_path(newPath);
	if(newPath.empty() == false && newPath.back() != '/')
		newPath += '/';
	return newPath;
}
void pragma::util::canonicalize_path(std::string &inOutPath)
{
	if(inOutPath.empty())
		return;
	std::replace(inOutPath.begin(), inOutPath.end(), '\\', '/');
	std::vector<std::string> components;
	string::explode(inOutPath, "/", components);

	auto first = true;
	auto startedWithSlash = false;
	auto endedWithSlash = false;

	for(auto it = components.begin(); it != components.end();) {
		auto &c = *it;
		string::remove_whitespace(c);
		if(first) {
			first = false;
			if(c.empty())
				startedWithSlash = true;
		}
		if(it == components.end() - 1 && c.empty())
			endedWithSlash = true;
		if(c.empty() || c == ".") {
			it = components.erase(it);
			continue;
		}
		if(c == "..") {
			if(it == components.begin()) {
				it = components.erase(it);
				continue;
			}
			auto idx = it - components.begin();
			components.erase(components.begin() + idx - 1, components.begin() + idx + 1);
			it = components.begin() + (idx - 1);
			continue;
		}
		++it;
	}

	std::string newPath;
	newPath.reserve(inOutPath.size());
	first = true;
	for(auto &c : components) {
		if(first)
			first = false;
		else
			newPath += '/';
		newPath += c;
	}
	if(startedWithSlash) // If our original path started with a simple slash, we want the new path to also start with it
		newPath = '/' + newPath;
	if(endedWithSlash) // If our original path ended with a simple slash, we want the new path to also end with it
		newPath += '/';
	if(newPath == "//")
		newPath = "/";
	inOutPath = std::move(newPath);
}

pragma::util::Path::Path(const std::string &path) { SetPath(path); }
pragma::util::Path::Path(std::string &&path) { SetPath(std::move(path)); }
pragma::util::Path::Path(const std::vector<std::string> &fromComponents)
{
	auto first = true;
	for(auto &c : fromComponents) {
		if(first)
			first = false;
		else
			m_path += '/';
		m_path += c;
	}
}
pragma::util::Path &pragma::util::Path::operator=(const std::string &path)
{
	SetPath(path);
	return *this;
}
pragma::util::Path &pragma::util::Path::operator=(std::string &&path)
{
	SetPath(std::move(path));
	return *this;
}

bool pragma::util::Path::operator==(const Path &other) const
{
	auto tmpThis = *this;
	tmpThis.Canonicalize();

	auto tmpOther = other;
	tmpOther.Canonicalize();
	return string::compare(tmpThis.m_path, tmpOther.m_path, USE_CASE_SENSITIVE_PATHS);
}
bool pragma::util::Path::operator==(const std::string &other) const
{
	Path pathOther {other};
	return *this == pathOther;
}
bool pragma::util::Path::operator!=(const Path &other) const { return !operator==(other); }
bool pragma::util::Path::operator!=(const std::string &other) const { return !operator==(other); }

pragma::util::Path pragma::util::Path::operator+(const Path &other) const
{
	auto cpy = *this;
	cpy += other;
	return cpy;
}
static void resolve_multi_slash(std::string &str)
{
	const std::string doubleSlash = "//";
	const std::string singleSlash = "/";
	size_t pos;
	while((pos = str.find(doubleSlash)) != std::string::npos)
		str.replace(pos, doubleSlash.length(), singleSlash);
}
pragma::util::Path &pragma::util::Path::operator+=(const Path &other)
{
	if(IsFile()) {
		if(other.GetString().find('/') != std::string::npos)
			return *this; // Appending a path to a file makes no sense, we'll just ignore it...
		m_path += other.m_path;
		resolve_multi_slash(m_path);
		return *this;
	}
	m_path += other.m_path;
	resolve_multi_slash(m_path);
	return *this;
}
pragma::util::Path pragma::util::Path::operator+(const char *other) const { return operator+(std::string {other}); }
pragma::util::Path &pragma::util::Path::operator+=(const char *other) { return operator+=(std::string {other}); }

std::string_view pragma::util::Path::GetPath() const
{
	if(IsFile() == false)
		return m_path;
	std::string_view path = m_path;
	auto br = path.rfind('/');
	return (br != std::string::npos) ? path.substr(0, br + 1) : std::string_view {};
}
std::string_view pragma::util::Path::GetFileName() const
{
	if(IsFile() == false)
		return "";
	std::string_view file = m_path;
	auto br = file.rfind('/');
	if(br != std::string::npos)
		file = file.substr(br + 1);
	return file;
}

std::string_view pragma::util::Path::GetFront() const { return GetComponent(0); }
std::string_view pragma::util::Path::GetBack() const
{
	auto br = m_path.rfind('/');
	auto isFile = IsFile();
	if(isFile == false) {
		br = m_path.rfind('/', br - 1);
		if(br == std::string::npos)
			return m_path;
	}
	if(br == std::string::npos) {
		if(isFile)
			return m_path;
		return std::string_view {};
	}
	return std::string_view {m_path}.substr(br + 1);
}
std::string_view pragma::util::Path::GetComponent(size_t offset, size_t *outOptNextOffset) const
{
	if(offset >= m_path.size())
		return {};
	std::string_view path {m_path};
	path = path.substr(offset);
	auto br = path.find('/');
	if(br == std::string::npos) {
		if(outOptNextOffset)
			*outOptNextOffset = std::numeric_limits<size_t>::max();
		return IsFile() ? path : std::string_view {};
	}
	if(outOptNextOffset)
		*outOptNextOffset = offset + br + 1;
	return path.substr(0, br);
}

void pragma::util::Path::PopFront()
{
	auto br = m_path.find('/');
	if(br == std::string::npos) {
		if(IsFile())
			m_path = "";
		return;
	}
	m_path = m_path.substr(br + 1);
}
void pragma::util::Path::PopBack()
{
	auto br = m_path.rfind('/');
	if(IsFile() == false) {
		br = m_path.rfind('/', br - 1);
		if(br == std::string::npos) {
			m_path = "";
			return;
		}
	}
	if(br == std::string::npos) {
		m_path = "";
		return;
	}
	m_path = m_path.substr(0, br + 1);
}
bool pragma::util::Path::MakeRelative(const Path &relativeTo)
{
	auto itThis = begin();
	auto itOther = relativeTo.begin();
	uint32_t numMatch = 0;
	while(itThis != end() && itOther != relativeTo.end() && *itThis == *itOther) {
		++numMatch;
		++itThis;
		++itOther;
	}
	if(itOther != relativeTo.end())
		return false;
	for(auto i = decltype(numMatch) {0u}; i < numMatch; ++i)
		PopFront();
	return true;
}

std::string pragma::util::Path::Move() { return std::move(m_path); }

uint32_t pragma::util::Path::GetComponentCount() const
{
	auto n = std::count(m_path.begin(), m_path.end(), '/');
	if(IsFile())
		++n;
	return n;
}
bool pragma::util::Path::IsEmpty() const { return m_path.empty(); }
std::vector<std::string> pragma::util::Path::ToComponents() const
{
	std::vector<std::string> components;
	string::explode(m_path, "/", components);
	return components;
}
const std::string &pragma::util::Path::GetString() const { return m_path; }
void pragma::util::Path::MoveUp() { PopBack(); }
void pragma::util::Path::Canonicalize() { canonicalize_path(m_path); }
void pragma::util::Path::RemoveLeadingRootSlash()
{
	if(m_path.empty() == false && m_path.front() == '/')
		m_path.erase(m_path.begin());
}
bool pragma::util::Path::IsFile() const { return !IsDirectory(); }
bool pragma::util::Path::IsDirectory() const { return m_path.empty() || m_path.back() == '/' || m_path.back() == '\\'; }
std::optional<std::string> pragma::util::Path::GetFileExtension() const
{
	std::string ext;
	return ufile::get_extension(m_path, &ext) ? ext : std::optional<std::string> {};
}
void pragma::util::Path::RemoveFileExtension() { ufile::remove_extension_from_filename(m_path); }

void pragma::util::Path::UpdatePath()
{
	std::replace(m_path.begin(), m_path.end(), '\\', '/');
	resolve_multi_slash(m_path);

#if 0
	if(m_path.empty() == false && m_path.front() == '/')
		m_path.erase(m_path.begin());
#endif
}
void pragma::util::Path::SetPath(std::string &&path)
{
	m_path = std::move(path);
	UpdatePath();
}
void pragma::util::Path::SetPath(const std::string &path)
{
	m_path = path;
	UpdatePath();
}

pragma::util::PathIterator<pragma::util::Path> pragma::util::Path::begin() { return PathIterator {*this, true}; }
pragma::util::PathIterator<pragma::util::Path> pragma::util::Path::end() { return PathIterator {*this, false}; }
pragma::util::PathIterator<const pragma::util::Path> pragma::util::Path::begin() const { return PathIterator {*this, true}; }
pragma::util::PathIterator<const pragma::util::Path> pragma::util::Path::end() const { return PathIterator {*this, false}; }

/*pragma::util::PathIterator<pragma::util::Path> pragma::util::Path::rbegin()
{
	return PathIterator{*this,false};
}
pragma::util::PathIterator<pragma::util::Path> pragma::util::Path::rend()
{
	return PathIterator{*this,true};
}
pragma::util::PathIterator<const pragma::util::Path> pragma::util::Path::rbegin() const
{
	return PathIterator{*this,false};
}
pragma::util::PathIterator<const pragma::util::Path> pragma::util::Path::rend() const
{
	return PathIterator{*this,true};
}*/

////////////

template<class TPath>
pragma::util::PathIterator<TPath>::PathIterator(TPath &path, bool begin) : m_path {&path}, m_pos {begin ? 0 : std::string::npos}
{
	if(m_path->GetString().empty()) {
		m_pos = std::string::npos;
		return;
	}
	if(begin)
		operator++();
}

template<class TPath>
const typename pragma::util::PathIterator<TPath>::value_type &pragma::util::PathIterator<TPath>::operator++()
{
	auto curPos = m_pos;
	if(curPos >= m_path->GetString().length()) {
		m_pos = std::string::npos;
		m_cur = {};
		return m_cur;
	}
	auto brPos = m_path->GetString().find('/', curPos);
	auto newPos = brPos;
	if(newPos == std::string::npos)
		newPos = m_path->GetString().length() - 1;
	if(curPos != std::string::npos) {
		if(newPos != std::string::npos)
			++newPos;
		if(newPos == curPos)
			m_cur = {};
		else
			m_cur = std::string_view {m_path->GetString()}.substr(curPos, (brPos != std::string::npos) ? (brPos - curPos) : brPos);
	}
	m_pos = (newPos <= m_path->GetString().length()) ? newPos : std::string::npos;
	return m_cur;
}

template<class TPath>
const typename pragma::util::PathIterator<TPath>::value_type &pragma::util::PathIterator<TPath>::operator++(int)
{
	return operator++();
}

template<class TPath>
const typename pragma::util::PathIterator<TPath>::value_type &pragma::util::PathIterator<TPath>::operator--()
{
	auto curPos = m_pos;
	if(curPos == 0)
		return m_cur;
	auto newPos = m_path->GetString().rfind('/', curPos);
	if(newPos == std::string::npos)
		newPos = 0;
	m_cur = std::string_view {m_path->GetString()}.substr(newPos + 1, (curPos != std::string::npos) ? (curPos - newPos - 1) : curPos);
	if(newPos != 0)
		--newPos; // Move to before the slash
	m_pos = newPos;
	return m_cur;
}

template<class TPath>
const typename pragma::util::PathIterator<TPath>::value_type &pragma::util::PathIterator<TPath>::operator--(int)
{
	return operator--();
}

template<class TPath>
const typename pragma::util::PathIterator<TPath>::value_type &pragma::util::PathIterator<TPath>::operator*() const
{
	return m_cur;
}

template<class TPath>
bool pragma::util::PathIterator<TPath>::operator==(const PathIterator &other) const
{
	return m_path == other.m_path && m_pos == other.m_pos;
}
template<class TPath>
bool pragma::util::PathIterator<TPath>::operator!=(const PathIterator &other) const
{
	return !operator==(other);
}

template class pragma::util::PathIterator<pragma::util::Path>;
template class pragma::util::PathIterator<const pragma::util::Path>;

////////////

pragma::util::Path operator+(const std::string &path, const pragma::util::Path &other)
{
	pragma::util::Path ppath {path};
	return ppath + other;
}

std::ostream &pragma::util::operator<<(std::ostream &out, const Path &path) { return out << path.GetString(); }

void pragma::util::Path::RunTests()
{
	auto check = [](const Path &path, const std::string &expectedString) {
		if(path.GetString() != expectedString)
			throw std::runtime_error {"util::Path test failed: Got path \"" + path.GetString() + "\", expected \"" + expectedString + "\"."};
	};
	check(CreatePath("a"), "a/");
	check(CreatePath("/"), "/");
	check(CreatePath("."), "./");
	check(CreatePath(".."), "../");

	check(CreatePath("a/b"), "a/b/");
	check(CreatePath("/a/b"), "/a/b/");

	check(CreatePath("/a//b"), "/a/b/");
	check(CreatePath("/a\\/b"), "/a/b/");
	check(CreatePath("/a/////b/"), "/a/b/");

	check(CreatePath("a", "b"), "a/b/");
	check(CreatePath("/a/", "b"), "/a/b/");
	check(CreatePath("a", "b", "c"), "a/b/c/");
	check(CreatePath("/a", "b", "c"), "/a/b/c/");

	check(CreatePath("", ""), "/");
	check(CreatePath("", "b"), "b/");
	check(CreatePath("a", ""), "a/");
	check(CreatePath("/", ""), "/");

	check(CreatePath("a/b/"), "a/b/");
	check(CreatePath("/a/b//"), "/a/b/");
	check(CreatePath("a///b//"), "a/b/");

	check(CreatePath("a\\b"), "a/b/");
	check(CreatePath("\\a\\b\\"), "/a/b/");

	check(CreatePath("./a"), "./a/");
	check(CreatePath("./"), "./");
	check(CreatePath("../a"), "../a/");

	// CreateFile test cases
	check(CreateFile("a", "b.txt"), "a/b.txt");
	check(CreateFile("/a", "b.txt"), "/a/b.txt");

	check(CreateFile("a", "b", "c.txt"), "a/b/c.txt");
	check(CreateFile("/a", "b", "c.txt"), "/a/b/c.txt");

	check(CreateFile("/a//b/", "c.txt"), "/a/b/c.txt");
	check(CreateFile("a///b", "c.txt"), "a/b/c.txt");

	check(CreateFile("a", "b/c.txt"), "a/b/c.txt");
	check(CreateFile("/a", "b/c.txt"), "/a/b/c.txt");

	check(CreateFile("a", "b"), "a/b");
	check(CreateFile("/a", "b"), "/a/b");
	check(CreateFile("a", "b.exe"), "a/b.exe");

	check(CreateFile("a\\b", "c.txt"), "a/b/c.txt");
	check(CreateFile("\\a\\b\\", "c.txt"), "/a/b/c.txt");

	check(CreateFile("", "b.txt"), "b.txt");
	check(CreateFile("a", ""), "a");

	check(CreateFile("./a", "b.txt"), "./a/b.txt");
	check(CreateFile("../a", "b.txt"), "../a/b.txt");

	check(CreatePath("a") + "b", "a/b");
	check(CreatePath("a") + "b/", "a/b/");
	check(CreatePath("a/") + "b", "a/b");
	check(CreatePath("a/") + "b/", "a/b/");

	// Addition
	check(CreatePath("a") + CreatePath("b"), "a/b/");
	check(CreatePath("/a") + CreatePath("b"), "/a/b/");
	check(CreatePath("/a/") + CreatePath("b/"), "/a/b/");
	check(CreatePath("a/") + CreatePath("b/"), "a/b/");

	check(CreatePath("a") + CreateFile("b"), "a/b");
	check(CreatePath("a/") + CreateFile("b"), "a/b");
	check(CreatePath("/a") + CreateFile("b.exe"), "/a/b.exe");
	check(CreatePath("a/") + CreateFile("c/b.exe"), "a/c/b.exe");

	check(CreateFile("a", "b.txt") + "c", "a/b.txtc");

	check(CreatePath("") + "b", "/b");
	check(CreatePath("") + "b/", "/b/");
	check(CreateFile("a", "") + "b", "ab");
	check(CreateFile("") + "b", "b");
	check(CreatePath("") + CreatePath("b"), "/b/");
	check(CreatePath("") + CreateFile("b"), "/b");
}
