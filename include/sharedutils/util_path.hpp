/* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef __UTIL_PATH_HPP__
#define __UTIL_PATH_HPP__

#include "utildefinitions.h"
#include <string>
#include <string_view>
#include <optional>
#include <vector>
#include <iostream>

namespace util
{
	DLLSHUTIL void canonicalize_path(std::string &inOutPath);

	class Path;
	template<class TPath>
		class DLLSHUTIL PathIterator
	{
	public:
		using iterator_category = std::forward_iterator_tag;
		using value_type = std::string_view;
		using difference_type = value_type;
		using pointer = value_type*;
		using reference = value_type&;

		PathIterator(TPath &value,bool begin);
		PathIterator(const PathIterator&)=default;
		PathIterator &operator=(const PathIterator&)=default;

		const value_type &operator++();
		const value_type &operator++(int);
		const value_type &operator--();
		const value_type &operator--(int);
		const value_type &operator*() const;

		bool operator==(const PathIterator &other) const;
		bool operator!=(const PathIterator &other) const;
	private:
		TPath *m_path = nllptr;
		std::string_view m_cur = {};
		size_t m_pos = 0;
	};

	class DLLSHUTIL Path
	{
	public:
		Path(const std::string &path="",std::optional<bool> optIsFile={});
		Path(const std::vector<std::string> &fromComponents);
		Path(const Path&)=default;
		Path &operator=(const Path&)=default;
		Path &operator=(const std::string &path);

		bool operator==(const Path &other) const;
		bool operator==(const std::string &other) const;
		bool operator!=(const Path &other) const;
		bool operator!=(const std::string &other) const;

		Path operator+(const Path &other);
		Path &operator+=(const Path &other);
		Path operator+(const char *other);
		Path &operator+=(const char *other);

		std::string GetPath() const;
		std::string GetFileName() const;

		std::string GetFront() const;
		std::string GetBack() const;
		void PopFront();
		void PopBack();
		void MakeRelative(const Path &relativeTo);

		uint32_t GetComponentCount() const;
		bool IsEmpty() const;
		std::vector<std::string> ToComponents() const;
		const std::string &GetString() const;
		void MoveUp();
		void Canonicalize();
		bool IsFile() const;
		std::optional<std::string> GetFileExtension() const;
		void RemoveFileExtension();

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
		void UpdateFileState(const std::string &path);
		std::string m_path = "";
		bool m_bFile = false;
	};
};

DLLSHUTIL util::Path operator+(const std::string &path,const util::Path &other);
DLLSHUTIL std::ostream &operator<<(std::ostream &out,const util::Path &path);

#endif