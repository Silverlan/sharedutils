/* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "sharedutils/util_image_buffer.hpp"
#include <mathutil/umath.h>

#pragma optimize("",off)
util::ImageBuffer::PixelView::PixelView(ImageBuffer &imgBuffer,Offset offset)
	: m_imageBuffer{imgBuffer},m_offset{offset}
{}
util::ImageBuffer::Offset util::ImageBuffer::PixelView::GetOffset() const {return m_offset;}
util::ImageBuffer::PixelIndex util::ImageBuffer::PixelView::GetPixelIndex() const {return m_imageBuffer.GetPixelIndex(GetX(),GetY());}
uint32_t util::ImageBuffer::PixelView::GetX() const {return GetPixelIndex() %m_imageBuffer.GetWidth();}
uint32_t util::ImageBuffer::PixelView::GetY() const {return GetPixelIndex() /m_imageBuffer.GetWidth();}
const void *util::ImageBuffer::PixelView::GetPixelData() const {return const_cast<PixelView*>(this)->GetPixelData();}
void *util::ImageBuffer::PixelView::GetPixelData() {return static_cast<uint8_t*>(m_imageBuffer.GetData()) +GetOffset();}
util::ImageBuffer::LDRValue util::ImageBuffer::PixelView::GetLDRValue(Channel channel) const
{
	if(channel == Channel::Alpha && m_imageBuffer.HasAlphaChannel() == false)
		return std::numeric_limits<LDRValue>::max();
	auto *data = static_cast<const void*>(static_cast<const uint8_t*>(GetPixelData()) +m_imageBuffer.GetChannelSize() *umath::to_integral(channel));
	switch(m_imageBuffer.GetFormat())
	{
	case Format::RGB8:
	case Format::RGBA8:
		return *static_cast<const LDRValue*>(data);
	case Format::RGB16:
	case Format::RGBA16:
		return ToLDRValue(*static_cast<const HDRValue*>(data));
	case Format::RGB32:
	case Format::RGBA32:
		return ToLDRValue(*static_cast<const FloatValue*>(data));
	}
	static_assert(umath::to_integral(Format::Count) == 7u);
	return 0;
}
util::ImageBuffer::HDRValue util::ImageBuffer::PixelView::GetHDRValue(Channel channel) const
{
	if(channel == Channel::Alpha && m_imageBuffer.HasAlphaChannel() == false)
		return umath::float32_to_float16_glm(std::numeric_limits<FloatValue>::max());
	auto *data = static_cast<const void*>(static_cast<const uint8_t*>(GetPixelData()) +m_imageBuffer.GetChannelSize() *umath::to_integral(channel));
	switch(m_imageBuffer.GetFormat())
	{
	case Format::RGB8:
	case Format::RGBA8:
		return ToHDRValue(*static_cast<const LDRValue*>(data));
	case Format::RGB16:
	case Format::RGBA16:
		return *static_cast<const HDRValue*>(data);
	case Format::RGB32:
	case Format::RGBA32:
		return ToHDRValue(*static_cast<const FloatValue*>(data));
	}
	static_assert(umath::to_integral(Format::Count) == 7u);
	return 0;
}
util::ImageBuffer::FloatValue util::ImageBuffer::PixelView::GetFloatValue(Channel channel) const
{
	if(channel == Channel::Alpha && m_imageBuffer.HasAlphaChannel() == false)
		return std::numeric_limits<FloatValue>::max();
	auto *data = static_cast<const void*>(static_cast<const uint8_t*>(GetPixelData()) +m_imageBuffer.GetChannelSize() *umath::to_integral(channel));
	switch(m_imageBuffer.GetFormat())
	{
	case Format::RGB8:
	case Format::RGBA8:
		return ToFloatValue(*static_cast<const LDRValue*>(data));
	case Format::RGB16:
	case Format::RGBA16:
		return ToFloatValue(*static_cast<const HDRValue*>(data));
	case Format::RGB32:
	case Format::RGBA32:
		return *static_cast<const FloatValue*>(data);
	}
	static_assert(umath::to_integral(Format::Count) == 7u);
	return 0.f;
}
void util::ImageBuffer::PixelView::SetValue(Channel channel,LDRValue value)
{
	if(channel == Channel::Alpha && m_imageBuffer.HasAlphaChannel() == false)
		return;
	auto *data = static_cast<void*>(static_cast<uint8_t*>(GetPixelData()) +m_imageBuffer.GetChannelSize() *umath::to_integral(channel));
	switch(m_imageBuffer.GetFormat())
	{
	case Format::RGB8:
	case Format::RGBA8:
		*static_cast<LDRValue*>(data) = value;
		return;
	case Format::RGB16:
	case Format::RGBA16:
		*static_cast<HDRValue*>(data) = ToHDRValue(value);
		return;
	case Format::RGB32:
	case Format::RGBA32:
		*static_cast<HDRValue*>(data) = ToFloatValue(value);
		return;
	}
	static_assert(umath::to_integral(Format::Count) == 7u);
}
void util::ImageBuffer::PixelView::SetValue(Channel channel,HDRValue value)
{
	if(channel == Channel::Alpha && m_imageBuffer.HasAlphaChannel() == false)
		return;
	auto *data = static_cast<void*>(static_cast<uint8_t*>(GetPixelData()) +m_imageBuffer.GetChannelSize() *umath::to_integral(channel));
	switch(m_imageBuffer.GetFormat())
	{
	case Format::RGB8:
	case Format::RGBA8:
		*static_cast<LDRValue*>(data) = ToLDRValue(value);
		return;
	case Format::RGB16:
	case Format::RGBA16:
		*static_cast<HDRValue*>(data) = value;
		return;
	case Format::RGB32:
	case Format::RGBA32:
		*static_cast<FloatValue*>(data) = ToFloatValue(value);
		return;
	}
	static_assert(umath::to_integral(Format::Count) == 7u);
}
void util::ImageBuffer::PixelView::SetValue(Channel channel,FloatValue value)
{
	if(channel == Channel::Alpha && m_imageBuffer.HasAlphaChannel() == false)
		return;
	auto *data = static_cast<void*>(static_cast<uint8_t*>(GetPixelData()) +m_imageBuffer.GetChannelSize() *umath::to_integral(channel));
	switch(m_imageBuffer.GetFormat())
	{
	case Format::RGB8:
	case Format::RGBA8:
		*static_cast<LDRValue*>(data) = ToLDRValue(value);
		return;
	case Format::RGB16:
	case Format::RGBA16:
		*static_cast<HDRValue*>(data) = ToHDRValue(value);
		return;
	case Format::RGB32:
	case Format::RGBA32:
		*static_cast<FloatValue*>(data) = value;
		return;
	}
	static_assert(umath::to_integral(Format::Count) == 7u);
}
util::ImageBuffer &util::ImageBuffer::PixelView::GetImageBuffer() const {return m_imageBuffer;}
void util::ImageBuffer::PixelView::CopyValue(Channel channel,const PixelView &outOther)
{
	auto format = outOther.GetImageBuffer().GetFormat();
	switch(format)
	{
	case Format::RGB8:
	case Format::RGBA8:
		SetValue(channel,outOther.GetLDRValue(channel));
		break;
	case Format::RGB16:
	case Format::RGBA16:
		SetValue(channel,outOther.GetHDRValue(channel));
		break;
	case Format::RGB32:
	case Format::RGBA32:
		SetValue(channel,outOther.GetFloatValue(channel));
		break;
	}
}
void util::ImageBuffer::PixelView::CopyValues(const PixelView &outOther)
{
	auto format = outOther.GetImageBuffer().GetFormat();
	auto numChannels = umath::to_integral(Channel::Count);
	for(auto i=decltype(numChannels){0u};i<numChannels;++i)
		CopyValue(static_cast<Channel>(i),outOther);
}

/////

util::ImageBuffer::PixelIterator::PixelIterator(ImageBuffer &imgBuffer,Offset offset)
	: m_pixelView{imgBuffer,offset}
{}
util::ImageBuffer::PixelIterator &util::ImageBuffer::PixelIterator::operator++()
{
	m_pixelView.m_offset = umath::min(m_pixelView.m_offset +m_pixelView.m_imageBuffer.GetPixelSize(),m_pixelView.m_imageBuffer.GetSize());
	return *this;
}
util::ImageBuffer::PixelIterator util::ImageBuffer::PixelIterator::operator++(int)
{
	auto it = *this;
	it.operator++();
	return it;
}
util::ImageBuffer::PixelView &util::ImageBuffer::PixelIterator::operator*() {return m_pixelView;}
util::ImageBuffer::PixelView *util::ImageBuffer::PixelIterator::operator->() {return &m_pixelView;}
bool util::ImageBuffer::PixelIterator::operator==(const PixelIterator &other) const
{
	return &other.m_pixelView.m_imageBuffer == &m_pixelView.m_imageBuffer && other.m_pixelView.m_offset == m_pixelView.m_offset;
}
bool util::ImageBuffer::PixelIterator::operator!=(const PixelIterator &other) const {return !operator==(other);}
#pragma optimize("",on)
