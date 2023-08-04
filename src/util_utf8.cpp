/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "sharedutils/util_utf8.hpp"
#include <sharedutils/tinyutf8/tinyutf8.h>
#include <iostream>

// TODO: Replace this implementation with https://github.com/DuffsDevice/tiny-utf8 once it has support for utf8 string_view

using namespace util;

Utf8StringView::Utf8StringView(const Utf8String &str) : m_string {str} {}
Utf8StringView::Utf8StringView(const std::string &str) : m_string {str} {}
Utf8StringView::Utf8StringView(const char *str) : m_string {str} {}
Utf8StringView::Utf8StringView(const char *str, size_t count) : m_string {str, count} {}
Utf8StringView &Utf8StringView::operator=(const Utf8String &str)
{
	m_string = str;
	return *this;
}
Utf8StringView &Utf8StringView::operator=(const Utf8StringView &str)
{
	m_string = str.m_string;
	return *this;
}
Utf8StringView &Utf8StringView::operator=(const std::string &str)
{
	m_string = str.c_str();
	return *this;
}
Utf8StringView &Utf8StringView::operator=(const char *str)
{
	m_string = str;
	return *this;
}
size_t Utf8StringView::length() const { return m_string.length(); }
size_t Utf8StringView::size() const { return m_string.size(); }
size_t Utf8StringView::find(const char *str, size_t startPos) const { return m_string.find(str, startPos); }
size_t Utf8StringView::find(const std::string &str, size_t startPos) const { return m_string.find(str, startPos); }
size_t Utf8StringView::find(const Utf8String &strU8, size_t startPos) const { return m_string.find(strU8, startPos); }
uint16_t Utf8StringView::get(size_t idx) const { return m_string.at(idx); }
Utf8StringIterator Utf8StringView::begin() const { return const_cast<Utf8StringView *>(this)->m_string.begin(); }
Utf8StringIterator Utf8StringView::end() const { return const_cast<Utf8StringView *>(this)->m_string.end(); }
std::string Utf8StringView::cpp_str() const { return m_string.cpp_str(); }
const char *Utf8StringView::c_str() const { return m_string.c_str(); }
const char *Utf8StringView::data() const { return m_string.data(); }
Utf8StringView::operator std::string() const { return cpp_str(); }
Utf8StringView Utf8StringView::substr(size_t start, size_t count) const { return m_string.substr(start, count); }
bool Utf8StringView::operator==(const Utf8StringView &other) const { return m_string == other.m_string; }
bool Utf8StringView::operator!=(const Utf8StringView &other) const { return !operator==(other); }

bool util::utf8_strncmp(const char *t0, const char *t1, size_t num)
{
	auto v0 = Utf8StringView {t0};
	auto v1 = Utf8StringView {t1};
	return v0.substr(0, num) == v1.substr(0, num);
}

//////////////////////////

util::Utf8String &operator+=(util::Utf8String &str, const util::Utf8StringView &view)
{
	str += view.to_str();
	return str;
}
std::ostream &operator<<(std::ostream &out, const util::Utf8String &str)
{
	out << str.cpp_str();
	return out;
}
std::ostream &operator<<(std::ostream &out, const util::Utf8StringView &str)
{
	out << std::string {str.c_str()};
	return out;
}
