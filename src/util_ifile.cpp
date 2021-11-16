/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "sharedutils/util_ifile.hpp"
#include <cstring>

#define IsEOF(c) (c == EOF || Eof())
std::string ufile::IFile::ReadLine()
{
	int32_t c;
	std::string name = "";
	c = ReadChar();
	if(c == '\n')
	{
		name += c;
		return name;
	}
	while(c != '\0' && !IsEOF(c))
	{
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
	while(str[pos] != '\0')
	{
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
	while(c != '\0' && !IsEOF(c) && !Eof())
	{
		name += c;
		c = Read<char>();
	}
	return name;
}

////////

ufile::MemoryFile::MemoryFile(uint8_t *data,size_t dataSize)
	: m_data{data},m_dataSize{dataSize}
{}
size_t ufile::MemoryFile::Read(void *data,size_t size)
{
	if(m_pos >= m_dataSize)
		return 0;
	size = umath::min(m_dataSize -m_pos,size);
	memcpy(data,m_data +m_pos,size);
	m_pos += size;
	return size;
}
size_t ufile::MemoryFile::Write(const void *data,size_t size)
{
	if(m_pos >= m_dataSize)
		return 0;
	size = umath::min(m_dataSize -m_pos,size);
	memcpy(m_data +m_pos,data,size);
	m_pos += size;
	return size;
}
size_t ufile::MemoryFile::Tell() {return m_pos;}
void ufile::MemoryFile::Seek(size_t offset,Whence whence)
{
	switch(whence)
	{
	case Whence::Set:
		m_pos = offset;
		break;
	case Whence::End:
		m_pos = m_dataSize +offset;
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
	Read(&c,sizeof(c));
	return c;
}
char *ufile::MemoryFile::GetMemoryDataPtr() {return reinterpret_cast<char*>(m_data +m_pos);}

////////

ufile::VectorFile::VectorFile()
	: MemoryFile{m_data.data(),m_data.size()}
{}
ufile::VectorFile::VectorFile(size_t size)
	: VectorFile{}
{Resize(size);}
ufile::VectorFile::VectorFile(std::vector<uint8_t> &&data)
	: MemoryFile{data.data(),data.size()},m_data{std::move(data)}
{}
const std::vector<uint8_t> &ufile::VectorFile::GetVector() const {return m_data;}
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
void ufile::VectorFile::Move(std::vector<uint8_t> &data) {data = std::move(m_data);}
