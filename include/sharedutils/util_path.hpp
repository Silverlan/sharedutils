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
		static Path CreatePath(const std::string &path);
		static Path CreateFile(const std::string &path);
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

		uint32_t GetComponentCount() const;
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
		void SetPath(const std::string &path);
		void SetPath(std::string &&path);
		void UpdatePath();
		std::string m_path = "";
	};
};

DLLSHUTIL util::Path operator+(const std::string &path, const util::Path &other);
DLLSHUTIL std::ostream &operator<<(std::ostream &out, const util::Path &path);

#endif
