// SPDX-FileCopyrightText: (c) 2019 Silverlan <opensource@pragma-engine.com>
// SPDX-License-Identifier: MIT

module;





module pragma.util;

import :data_stream;

util::DataStreamBase::DataStreamBase(uint32_t size, uint32_t headerSize) : m_data(new std::vector<uint8_t>()), m_offset(0), m_size(new uint32_t(size)), m_dataSize(size), m_headerSize(headerSize)
{
	m_data->resize(headerSize);
	m_rawData = m_data.get()->data() + headerSize;
}

util::DataStreamBase::DataStreamBase() : m_data(new std::vector<uint8_t>()), m_offset(0), m_size(nullptr), m_dataSize(0) { m_rawData = m_data.get()->data(); }

util::DataStreamBase::DataStreamBase(void *data, uint32_t size) : m_data(nullptr), m_rawData(static_cast<uint8_t *>(data)), m_size(std::make_unique<uint32_t>(size)), m_dataSize(size) {}

util::DataStreamBase::~DataStreamBase()
{
	m_data = nullptr;
	m_size = nullptr;
}

void util::DataStreamBase::SetHeaderSize(uint32_t sz)
{
	if(m_data == nullptr)
		throw std::logic_error("Unable to change header size for external memory data!");
	m_headerSize = sz;
	m_data->resize(m_headerSize);
	m_rawData = m_data.get()->data() + m_headerSize;
}

uint32_t util::DataStreamBase::GetHeaderSize() const { return m_headerSize; }
void util::DataStreamBase::SetHeaderData(const void *data)
{
	if(m_data->size() < GetHeaderSize())
		throw std::out_of_range("Header data out of data stream range!");
	memcpy(m_data->data(), data, GetHeaderSize());
}
void util::DataStreamBase::Reserve(uint32_t size)
{
	if(m_data == nullptr)
		return;
	m_data->reserve(size);
}

void util::DataStreamBase::Resize(uint32_t sz, bool bForceResize)
{
	if(m_data == nullptr || (bForceResize == false && sz + m_headerSize <= m_data->size()))
		return;
	m_data->resize(sz + m_headerSize);
	m_rawData = m_data->data() + m_headerSize;
	if(m_offset > m_data->size() - m_headerSize)
		m_offset = static_cast<uint32_t>(m_data->size() - m_headerSize);
}

void util::DataStreamBase::Write(const uint8_t *c, uint32_t size)
{
	Resize(m_offset + size);
	size = GetClampedSize(size);
	if(size == 0)
		return;
	memcpy(&m_rawData[m_offset], &c[0], size);
	m_offset += size;
	m_dataSize = umath::max(m_dataSize, m_offset);
}
void util::DataStreamBase::Write(const uint8_t *c, uint32_t size, uint32_t pos)
{
	Resize(pos + size);
	size = GetClampedSize(size);
	if(size == 0)
		return;
	memcpy(&m_rawData[pos], &c[0], size);
	m_dataSize = umath::max(m_dataSize, pos + size);
}
util::DataStreamBase &util::DataStreamBase::operator<<(const std::string &str)
{
	WriteString(str);
	return *this;
}
util::DataStreamBase &util::DataStreamBase::operator<<(const char *str)
{
	WriteString(str);
	return *this;
}
void util::DataStreamBase::SetOffset(uint32_t offset) { m_offset = offset; }

void util::DataStreamBase::SetMessageSize(uint32_t size)
{
	if(m_size == nullptr)
		m_size = std::make_unique<uint32_t>();
	*m_size = size;
}

void util::DataStreamBase::Invalidate()
{
	if(m_size != nullptr)
		*m_size = 0;
	m_dataSize = 0;
	m_offset = 0;
}

uint8_t *util::DataStreamBase::GetData(bool bIncludeHeaderData) { return (bIncludeHeaderData == false) ? m_rawData : (m_rawData - GetHeaderSize()); }
uint32_t util::DataStreamBase::GetDataSize() const { return m_dataSize; }
uint32_t util::DataStreamBase::GetInternalSize() const { return static_cast<uint32_t>((m_data != nullptr) ? m_data->size() : m_dataSize); }
uint32_t util::DataStreamBase::GetClampedSize(uint32_t sz) const { return GetClampedSize(sz, m_offset); }
uint32_t util::DataStreamBase::GetClampedSize(uint32_t sz, uint32_t offset) const
{
	auto dataSize = (m_data != nullptr) ? (m_data->size() - m_headerSize) : m_dataSize;
	return static_cast<uint32_t>(((offset + sz) >= dataSize) ? (dataSize - offset) : sz);
}

void util::DataStreamBase::WriteString(const std::string &str, Bool bNullTerminated)
{
	auto len = str.length();
	Resize(static_cast<uint32_t>(m_offset + len + 1));
	auto lenClamped = GetClampedSize(static_cast<uint32_t>(len));
	if(lenClamped > 0)
		memcpy(&m_rawData[m_offset], &str[0], lenClamped);
	m_offset += static_cast<uint32_t>(lenClamped);
	if(bNullTerminated == true && lenClamped == len) {
		m_rawData[m_offset] = '\0';
		m_offset++;
	}
	m_dataSize = umath::max(m_dataSize, m_offset);
}

void util::DataStreamBase::WriteString(const char *str, Bool bNullTerminated)
{
	std::string s(str);
	WriteString(s, bNullTerminated);
}

void util::DataStreamBase::AdjustSize(uint32_t offset, uint32_t &sz)
{
	if(m_data == nullptr)
		return;
	size_t szData = m_data->size() - m_headerSize;
	if(offset >= szData) {
		sz = 0;
		return;
	}
	if(offset + sz > szData)
		sz = static_cast<uint32_t>(szData) - offset;
}

void util::DataStreamBase::Read(void *dst, uint32_t size)
{
	if(Eof()) {
		memset(dst, 0, size);
		return;
	}
	auto szOriginal = size;
	AdjustSize(m_offset, size);
	size = GetClampedSize(size);
	if(size > 0)
		memcpy(dst, &m_rawData[m_offset], size);
	if(size < szOriginal)
		memset(static_cast<uint8_t *>(dst) + size, 0, szOriginal - size);
	m_offset += size;
}

bool util::DataStreamBase::Eof() { return (m_offset >= ((m_data != nullptr) ? m_data->size() : m_dataSize)) ? true : false; }

std::string util::DataStreamBase::ReadUntil(const std::string &pattern)
{
	if(Eof() || pattern.empty())
		return "";
	std::string r;
	auto lenPattern = pattern.size();
	size_t match = 0;
	size_t len = 0;
	do {
		char c;
		Read(&c, sizeof(char));
		r += c;
		len++;
		if(r[len - 1] == pattern[match])
			match++;
		else
			match = 0;
	} while(match < lenPattern && !Eof());
	return r;
}
std::string util::DataStreamBase::ReadLine() { return ReadUntil("\n"); }
std::string util::DataStreamBase::ReadString()
{
	if(Eof())
		return "";
	std::string r;
	char c;
	Read(&c, sizeof(char));
	while(c != '\0') {
		r += c;
		if(Eof())
			break;
		Read(&c, sizeof(char));
	}
	return r;
}
std::string util::DataStreamBase::ReadString(UInt32 len)
{
	if(Eof() || len == 0)
		return "";
	std::string r;
	r.reserve(len);
	for(auto i = decltype(len) {0u}; i < len; ++i) {
		char c;
		Read(&c, sizeof(char));
		r += c;
		if(Eof())
			break;
	}
	return r;
}
util::DataStreamBase &util::DataStreamBase::operator>>(std::string &str)
{
	str = ReadString();
	return *this;
}

uint32_t util::DataStreamBase::GetSize() const
{
	if(m_size != nullptr)
		return *m_size;
	return m_offset;
}
uint32_t util::DataStreamBase::GetOffset() const { return m_offset; }

std::ostream &operator<<(std::ostream &out, const util::DataStream &o)
{
	out << "DataStream";
	out << "[Size:" << o->GetSize() << "]";
	return out;
}
