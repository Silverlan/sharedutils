// SPDX-FileCopyrightText: (c) 2021 Silverlan <opensource@pragma-engine.com>
// SPDX-License-Identifier: MIT

module;


#include "sharedutils/magic_enum.hpp"

module pragma.util;

import :file_interface;

#define IsEOF(c) (c == EOF || Eof())
std::string ufile::IFile::ReadLine()
{
	int32_t c;
	std::string name = "";
	c = ReadChar();
	if(c == '\n') {
		name += c;
		return name;
	}
	while(c != '\0' && !IsEOF(c)) {
		name += c;
		c = ReadChar();
		if(Eof() || c == '\n')
			break;
	}
	return name;
}

int32_t ufile::IFile::WriteString(const std::string &str)
{
	unsigned int pos = 0;
	while(str[pos] != '\0') {
		Write<char>(str[pos]);
		pos++;
	}
	return 0;
}

std::string ufile::IFile::ReadString()
{
	unsigned char c;
	std::string name = "";
	c = Read<char>();
	while(c != '\0' && !Eof()) {
		name += c;
		c = Read<char>();
	}
	return name;
}

////////

ufile::MemoryFile::MemoryFile(uint8_t *data, size_t dataSize) : m_data {data}, m_dataSize {dataSize} {}
size_t ufile::MemoryFile::Read(void *data, size_t size)
{
	if(m_pos >= m_dataSize)
		return 0;
	size = umath::min(m_dataSize - m_pos, size);
	memcpy(data, m_data + m_pos, size);
	m_pos += size;
	return size;
}
size_t ufile::MemoryFile::Write(const void *data, size_t size)
{
	if(m_pos >= m_dataSize)
		return 0;
	size = umath::min(m_dataSize - m_pos, size);
	memcpy(m_data + m_pos, data, size);
	m_pos += size;
	return size;
}
size_t ufile::MemoryFile::Tell() { return m_pos; }
void ufile::MemoryFile::Seek(size_t offset, Whence whence)
{
	switch(whence) {
	case Whence::Set:
		m_pos = offset;
		break;
	case Whence::End:
		m_pos = m_dataSize + offset;
		break;
	case Whence::Cur:
		m_pos += offset;
		break;
	}
}
int32_t ufile::MemoryFile::ReadChar()
{
	if(m_pos >= m_dataSize)
		return EOF;
	char c;
	Read(&c, sizeof(c));
	return c;
}
char *ufile::MemoryFile::GetMemoryDataPtr() { return reinterpret_cast<char *>(m_data + m_pos); }

////////

ufile::VectorFile::VectorFile() : MemoryFile {nullptr, 0} {}
ufile::VectorFile::VectorFile(size_t size) : VectorFile {} { Resize(size); }
ufile::VectorFile::VectorFile(std::vector<uint8_t> &&data) : MemoryFile {data.data(), data.size()}, m_data {std::move(data)} {}
const std::vector<uint8_t> &ufile::VectorFile::GetVector() const { return m_data; }
void ufile::VectorFile::Resize(size_t size)
{
	m_data.resize(size);
	MemoryFile::m_data = m_data.data();
	m_dataSize = size;
}
ufile::VectorFile &ufile::VectorFile::operator=(std::vector<uint8_t> &&data)
{
	m_data = std::move(data);
	return *this;
}
void ufile::VectorFile::Move(std::vector<uint8_t> &data) { data = std::move(m_data); }

////////

ufile::BaseStreamFile::BaseStreamFile(std::ios_base::openmode openMode) : m_stream {openMode} {}
ufile::BaseStreamFile::BaseStreamFile(std::stringstream &&stream) : m_stream {std::move(stream)} {}
std::stringstream ufile::BaseStreamFile::MoveStream() { return std::move(m_stream); }

////////

ufile::InStreamFile::InStreamFile() : BaseStreamFile {std::stringstream::in | std::stringstream::binary} {}
ufile::InStreamFile::InStreamFile(std::stringstream &&stream) : BaseStreamFile {std::move(stream)} {}
size_t ufile::InStreamFile::Read(void *data, size_t size)
{
	m_stream.read(static_cast<char *>(data), size);
	return size;
}
size_t ufile::InStreamFile::Write(const void *data, size_t size) { return 0; }
size_t ufile::InStreamFile::Tell() { return m_stream.tellg(); }
void ufile::InStreamFile::Seek(size_t offset, Whence whence)
{
	std::ios_base::seekdir seekdir;
	switch(whence) {
	case Whence::Set:
		seekdir = std::ios_base::beg;
		break;
	case Whence::Cur:
		seekdir = std::ios_base::cur;
		break;
	case Whence::End:
		seekdir = std::ios_base::end;
		break;
	default:
		throw std::runtime_error {"Unknown whence enum " + std::string {magic_enum::enum_name(whence)}};
	}
	m_stream.seekg(offset, seekdir);
}
int32_t ufile::InStreamFile::ReadChar()
{
	char c = '\0';
	m_stream.read(&c, 1);
	return c;
}
size_t ufile::InStreamFile::GetSize()
{
	auto oldPos = Tell();
	Seek(0, Whence::End);
	auto size = Tell();
	Seek(oldPos);
	return size;
}

////////

ufile::OutStreamFile::OutStreamFile() : BaseStreamFile {std::stringstream::out | std::stringstream::binary} {}
ufile::OutStreamFile::OutStreamFile(std::stringstream &&stream) : BaseStreamFile {std::move(stream)} {}
size_t ufile::OutStreamFile::Read(void *data, size_t size) { return 0; }
size_t ufile::OutStreamFile::Write(const void *data, size_t size)
{
	m_stream.write(static_cast<const char *>(data), size);
	return size;
}
size_t ufile::OutStreamFile::Tell() { return m_stream.tellp(); }
void ufile::OutStreamFile::Seek(size_t offset, Whence whence)
{
	std::ios_base::seekdir seekdir;
	switch(whence) {
	case Whence::Set:
		seekdir = std::ios_base::beg;
		break;
	case Whence::Cur:
		seekdir = std::ios_base::cur;
		break;
	case Whence::End:
		seekdir = std::ios_base::end;
		break;
	default:
		throw std::runtime_error {"Unknown whence enum " + std::string {magic_enum::enum_name(whence)}};
	}
	m_stream.seekp(offset, seekdir);
}
int32_t ufile::OutStreamFile::ReadChar() { return 0; }
size_t ufile::OutStreamFile::GetSize()
{
	auto oldPos = Tell();
	Seek(0, Whence::End);
	auto size = Tell();
	Seek(oldPos);
	return size;
}

////////

ufile::FileWrapper::FileWrapper() {}
ufile::FileWrapper::FileWrapper(const std::shared_ptr<IFile> &f) : m_file {f} {}
size_t ufile::FileWrapper::Read(void *data, size_t size) { return m_file->Read(data, size); }
size_t ufile::FileWrapper::Write(const void *data, size_t size) { return m_file->Write(data, size); }
size_t ufile::FileWrapper::Tell() { return m_file->Tell(); }
void ufile::FileWrapper::Seek(size_t offset, Whence whence) { return m_file->Seek(offset, whence); }
int32_t ufile::FileWrapper::ReadChar() { return m_file->ReadChar(); }
size_t ufile::FileWrapper::GetSize() { return m_file->GetSize(); }
bool ufile::FileWrapper::Eof() { return m_file->Eof(); }
std::optional<std::string> ufile::FileWrapper::GetFileName() const { return m_file->GetFileName(); }
