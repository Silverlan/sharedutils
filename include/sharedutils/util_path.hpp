/* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef __UTIL_PATH_HPP__
#define __UTIL_PATH_HPP__

#include "utildefinitions.h"
#include "sharedutils/util_file.h"
#include <string>
#include <string_view>
#include <optional>
#include <vector>
#include <iostream>
#include <cinttypes>

#undef CreateFile

namespace util {
	DLLSHUTIL void canonicalize_path(std::string &inOutPath);
	DLLSHUTIL std::string get_normalized_path(const std::string &path);

	class Path;
	template<class TPath>
	class DLLSHUTIL PathIterator {
	  public:
		using iterator_category = std::forward_iterator_tag;
		using value_type = std::string_view;
		using difference_type = value_type;
		using pointer = value_type *;
		using reference = value_type &;

		PathIterator(TPath &value, bool begin);
		PathIterator(const PathIterator &) = default;
		PathIterator &operator=(const PathIterator &) = default;

		const value_type &operator++();
		const value_type &operator++(int);
		const value_type &operator--();
		const value_type &operator--(int);
		const value_type &operator*() const;

		bool operator==(const PathIterator &other) const;
		bool operator!=(const PathIterator &other) const;
	  private:
		TPath *m_path = nullptr;
		std::string_view m_cur = {};
		size_t m_pos = 0;
	};

	class DLLSHUTIL Path {
	  public:
		template<typename... Args>
		static Path CreatePath(const Args &...args)
		{
			auto isFirstValidArg = false;
			auto npath = Concatenate(isFirstValidArg, args...);
			if(npath.empty())
				npath = '/';
			else if(npath.back() != '/' && npath.back() != '\\')
				npath += '/';
			return util::Path {std::move(npath)};
		}
		template<typename... Args>
		static Path CreateFile(const Args &...args)
		{
			auto isFirstValidArg = false;
			auto strPath = Concatenate(isFirstValidArg, args...);
			util::Path path {std::move(strPath)};
			auto &p = reinterpret_cast<std::string &>(path);
			if(!p.empty() && (p.back() == '/' || p.back() == '\\'))
				p.pop_back();
			return path;
		}
		static void RunTests();
		Path(const std::string &path = "");
		Path(std::string &&path);
		Path(const std::vector<std::string> &fromComponents);
		Path(const Path &) = default;
		Path &operator=(const Path &) = default;
		Path &operator=(const std::string &path);
		Path &operator=(std::string &&path);

		bool operator==(const Path &other) const;
		bool operator==(const std::string &other) const;
		bool operator!=(const Path &other) const;
		bool operator!=(const std::string &other) const;

		Path operator+(const Path &other) const;
		Path &operator+=(const Path &other);
		Path operator+(const char *other) const;
		Path &operator+=(const char *other);

		std::string_view GetPath() const;
		std::string_view GetFileName() const;

		std::string_view GetFront() const;
		std::string_view GetBack() const;
		std::string_view GetComponent(size_t offset, size_t *outOptNextOffset = nullptr) const;
		void PopFront();
		void PopBack();
		bool MakeRelative(const Path &relativeTo);

		std::uint32_t GetComponentCount() const;
		bool IsEmpty() const;
		std::vector<std::string> ToComponents() const;
		const std::string &GetString() const;
		void MoveUp();
		void Canonicalize();
		bool IsFile() const;
		bool IsDirectory() const;
		std::optional<std::string> GetFileExtension() const;
		void RemoveFileExtension();
		template<typename TList>
		void RemoveFileExtension(const TList &extensions)
		{
			ufile::remove_extension_from_filename(m_path, extensions);
		}

		std::string Move();
		PathIterator<Path> begin();
		PathIterator<Path> end();
		PathIterator<const Path> begin() const;
		PathIterator<const Path> end() const;
		/*PathIterator<Path> rbegin();
		PathIterator<Path> rend();
		PathIterator<const Path> rbegin() const;
		PathIterator<const Path> rend() const;*/
	  private:
		template<typename First, typename... Rest>
		static std::string Concatenate(bool &isFirstValidArg, const First &first)
		{
			auto str = to_string(first);
			// We only want to begin the string with a forward slash if the first valid argument (i.e. non-empty)
			// starts with a forward slash as well (which indicates an absolute path).
			if(!isFirstValidArg) {
				if(!str.empty()) {
					isFirstValidArg = true;
					return str;
				}
			}
			if(str.empty() || str.front() == '/' || str.front() == '\\')
				return str;
			return std::string {"/"} + str;
		}
		template<typename First, typename... Rest>
		static std::string Concatenate(bool &isFirstValidArg, const First &first, const Rest &...rest)
		{
			auto s0 = Concatenate(isFirstValidArg, first);
			auto s1 = Concatenate(isFirstValidArg, rest...);
			return s0 + s1;
		}
		static std::string to_string(const std::string &s) { return s; }
		static std::string to_string(const std::string_view &sw) { return std::string {sw}; }
		static std::string to_string(const char *s) { return std::string {s}; }
		static std::string to_string(const util::Path &path) { return path.GetString(); }
		template<typename T>
		static std::string to_string(const T &value)
		{
			return std::to_string(value);
		}
		void SetPath(const std::string &path);
		void SetPath(std::string &&path);
		void UpdatePath();
		std::string m_path = "";
	};

	template<typename... Args>
	Path FilePath(const Args &...args)
	{
		return Path::CreateFile(args...);
	}

	template<typename... Args>
	Path DirPath(const Args &...args)
	{
		return Path::CreatePath(args...);
	}
};

DLLSHUTIL util::Path operator+(const std::string &path, const util::Path &other);
DLLSHUTIL std::ostream &operator<<(std::ostream &out, const util::Path &path);

#endif
