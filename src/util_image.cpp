/* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "sharedutils/util_image_buffer.hpp"
#include <mathutil/umath.h>
#include <stdexcept>

#pragma optimize("",off)
std::shared_ptr<util::ImageBuffer> util::ImageBuffer::Create(const void *data,uint32_t width,uint32_t height,Format format)
{
	return Create(const_cast<void*>(data),width,height,format,false);
}
std::shared_ptr<util::ImageBuffer> util::ImageBuffer::Create(void *data,uint32_t width,uint32_t height,Format format,bool ownedExternally)
{
	if(ownedExternally == false)
	{
		auto buf = Create(width,height,format);
		if(data)
			buf->Write(0,width *height *GetPixelSize(format),data);
		return buf;
	}
	auto ptrData = std::shared_ptr<void>{data,[](void*) {}};
	return std::shared_ptr<ImageBuffer>{new ImageBuffer{ptrData,width,height,format}};
}
std::shared_ptr<util::ImageBuffer> util::ImageBuffer::Create(uint32_t width,uint32_t height,Format format)
{
	auto buf = std::shared_ptr<ImageBuffer>{new ImageBuffer{nullptr,width,height,format}};
	buf->Reallocate();
	return buf;
}
util::ImageBuffer::LDRValue util::ImageBuffer::ToLDRValue(HDRValue value)
{
	return umath::clamp<uint32_t>(
		umath::float16_to_float32_glm(value) *static_cast<float>(std::numeric_limits<uint8_t>::max()),
		0,std::numeric_limits<uint8_t>::max()
	);
}
util::ImageBuffer::LDRValue util::ImageBuffer::ToLDRValue(FloatValue value)
{
	return umath::clamp<uint32_t>(
		value *static_cast<float>(std::numeric_limits<uint8_t>::max()),
		0,std::numeric_limits<uint8_t>::max()
	);
}
util::ImageBuffer::HDRValue util::ImageBuffer::ToHDRValue(LDRValue value)
{
	return ToHDRValue(ToFloatValue(value));
}
util::ImageBuffer::HDRValue util::ImageBuffer::ToHDRValue(FloatValue value)
{
	return umath::float32_to_float16_glm(value);
}
util::ImageBuffer::FloatValue util::ImageBuffer::ToFloatValue(LDRValue value)
{
	return value /static_cast<float>(std::numeric_limits<LDRValue>::max());
}
util::ImageBuffer::FloatValue util::ImageBuffer::ToFloatValue(HDRValue value)
{
	return umath::float16_to_float32_glm(value);
}
void util::ImageBuffer::FlipHorizontally()
{
	auto imgFlipped = Copy();
	auto srcPxView = imgFlipped->GetPixelView();
	auto dstPxView = GetPixelView();
	auto w = GetWidth();
	auto h = GetHeight();
	for(auto x=decltype(w){0u};x<w;++x)
	{
		for(auto y=decltype(h){0};y<h;++y)
		{
			imgFlipped->InitPixelView(w -x -1,y,srcPxView);
			InitPixelView(x,y,dstPxView);
			dstPxView.CopyValues(srcPxView);
		}
	}
}
void util::ImageBuffer::FlipVertically()
{
	auto imgFlipped = Copy();
	auto srcPxView = imgFlipped->GetPixelView();
	auto dstPxView = GetPixelView();
	auto w = GetWidth();
	auto h = GetHeight();
	for(auto x=decltype(w){0u};x<w;++x)
	{
		for(auto y=decltype(h){0};y<h;++y)
		{
			imgFlipped->InitPixelView(x,h -y -1,srcPxView);
			InitPixelView(x,y,dstPxView);
			dstPxView.CopyValues(srcPxView);
		}
	}
}
void util::ImageBuffer::InitPixelView(uint32_t x,uint32_t y,PixelView &pxView)
{
	pxView.m_offset = GetPixelOffset(x,y);
}
util::ImageBuffer::PixelView util::ImageBuffer::GetPixelView()
{
	return PixelView{*this,0};
}
uint8_t util::ImageBuffer::GetChannelCount(Format format)
{
	switch(format)
	{
	case Format::RGB8:
	case Format::RGB16:
	case Format::RGB32:
		return 3;
	case Format::RGBA8:
	case Format::RGBA16:
	case Format::RGBA32:
		return 4;
	}
	static_assert(umath::to_integral(Format::Count) == 7u);
	return 0;
}
uint8_t util::ImageBuffer::GetChannelSize(Format format)
{
	switch(format)
	{
	case Format::RGB8:
	case Format::RGBA8:
		return 1;
	case Format::RGB16:
	case Format::RGBA16:
		return 2;
	case Format::RGB32:
	case Format::RGBA32:
		return 4;
	}
	static_assert(umath::to_integral(Format::Count) == 7u);
	return 0;
}
util::ImageBuffer::Size util::ImageBuffer::GetPixelSize(Format format)
{
	return GetChannelCount(format) *GetChannelSize(format);
}
util::ImageBuffer::ImageBuffer(const std::shared_ptr<void> &data,uint32_t width,uint32_t height,Format format)
	: m_data{data},m_width{width},m_height{height},m_format{format}
{}
std::shared_ptr<util::ImageBuffer> util::ImageBuffer::Copy() const
{
	return util::ImageBuffer::Create(m_data.get(),m_width,m_height,m_format,false);
}
std::shared_ptr<util::ImageBuffer> util::ImageBuffer::Copy(Format format) const
{
	// Optimized copy that performs copy +format change in one go
	auto cpy = util::ImageBuffer::Create(nullptr,m_width,m_height,m_format,true);
	Convert(const_cast<ImageBuffer&>(*this),*cpy,format);
	return cpy;
}
util::ImageBuffer::Format util::ImageBuffer::GetFormat() const {return m_format;}
uint32_t util::ImageBuffer::GetWidth() const {return m_width;}
uint32_t util::ImageBuffer::GetHeight() const {return m_height;}
util::ImageBuffer::Size util::ImageBuffer::GetPixelSize() const {return GetPixelSize(GetFormat());}
uint32_t util::ImageBuffer::GetPixelCount() const {return m_width *m_height;}
bool util::ImageBuffer::HasAlphaChannel() const {return GetChannelCount() >= umath::to_integral(Channel::Alpha);}
bool util::ImageBuffer::IsLDRFormat() const
{
	switch(GetFormat())
	{
	case Format::RGB_LDR:
	case Format::RGBA_LDR:
		return true;
	}
	return false;
}
bool util::ImageBuffer::IsHDRFormat() const
{
	switch(GetFormat())
	{
	case Format::RGB_HDR:
	case Format::RGBA_HDR:
		return true;
	}
	return false;
}
bool util::ImageBuffer::IsFloatFormat() const
{
	switch(GetFormat())
	{
	case Format::RGB_FLOAT:
	case Format::RGBA_FLOAT:
		return true;
	}
	return false;
}
uint8_t util::ImageBuffer::GetChannelCount() const {return GetChannelCount(GetFormat());}
uint8_t util::ImageBuffer::GetChannelSize() const {return GetChannelSize(GetFormat());}
util::ImageBuffer::PixelIndex util::ImageBuffer::GetPixelIndex(uint32_t x,uint32_t y) const {return y *GetWidth() +x;}
util::ImageBuffer::Offset util::ImageBuffer::GetPixelOffset(uint32_t x,uint32_t y) const {return GetPixelIndex(x,y) *GetPixelSize();}
const void *util::ImageBuffer::GetData() const {return const_cast<ImageBuffer*>(this)->GetData();}
void *util::ImageBuffer::GetData() {return m_data.get();}
void util::ImageBuffer::Reallocate()
{
	m_data = std::shared_ptr<void>{new uint8_t[GetSize()],[](const void *ptr) {
		delete[] static_cast<const uint8_t*>(ptr);
	}};
}
util::ImageBuffer::PixelIterator util::ImageBuffer::begin()
{
	return PixelIterator{*this,0};
}
util::ImageBuffer::PixelIterator util::ImageBuffer::end()
{
	return PixelIterator{*this,GetSize()};
}
void util::ImageBuffer::Convert(ImageBuffer &srcImg,ImageBuffer &dstImg,Format targetFormat)
{
	if(dstImg.GetFormat() == targetFormat)
		return;
	dstImg.m_format = targetFormat;
	dstImg.Reallocate();
	auto itSrc = srcImg.begin();
	auto itDst = dstImg.begin();
	while(itSrc != srcImg.end())
	{
		auto &srcPixel = *itSrc;
		auto &dstPixel = *itDst;
		auto numChannels = umath::to_integral(Channel::Count);
		for(auto i=decltype(numChannels){0u};i<numChannels;++i)
		{
			auto channel = static_cast<Channel>(i);
			switch(targetFormat)
			{
			case Format::RGB8:
			case Format::RGBA8:
				dstPixel.SetValue(channel,srcPixel.GetLDRValue(channel));
				break;
			case Format::RGB16:
			case Format::RGBA16:
				dstPixel.SetValue(channel,srcPixel.GetHDRValue(channel));
				break;
			case Format::RGB32:
			case Format::RGBA32:
				dstPixel.SetValue(channel,srcPixel.GetFloatValue(channel));
				break;
			}
		}
		++itSrc;
		++itDst;
	}
}
void util::ImageBuffer::Convert(Format targetFormat)
{
	if(GetFormat() == targetFormat)
		return;
	auto cpy = *this;
	Convert(cpy,*this,targetFormat);
}
void util::ImageBuffer::ToLDR()
{
	switch(m_format)
	{
	case Format::RGB16:
	case Format::RGB32:
		Convert(Format::RGB8);
		return;
	case Format::RGBA16:
	case Format::RGBA32:
		Convert(Format::RGBA8);
		return;
	}
	static_assert(umath::to_integral(Format::Count) == 7u);
}
void util::ImageBuffer::ToHDR()
{
	switch(m_format)
	{
	case Format::RGB8:
	case Format::RGB32:
		Convert(Format::RGB16);
		return;
	case Format::RGBA8:
	case Format::RGBA32:
		Convert(Format::RGBA16);
		return;
	}
	static_assert(umath::to_integral(Format::Count) == 7u);
}
void util::ImageBuffer::ToFloat()
{
	switch(m_format)
	{
	case Format::RGB8:
	case Format::RGB16:
		Convert(Format::RGB32);
		return;
	case Format::RGBA8:
	case Format::RGBA16:
		Convert(Format::RGBA32);
		return;
	}
	static_assert(umath::to_integral(Format::Count) == 7u);
}
util::ImageBuffer::Size util::ImageBuffer::GetSize() const
{
	return GetPixelCount() *GetPixelSize(GetFormat());
}
void util::ImageBuffer::Read(Offset offset,Size size,void *outData)
{
	auto *srcPtr = static_cast<uint8_t*>(m_data.get()) +offset;
	memcpy(outData,srcPtr,size);
}
void util::ImageBuffer::Write(Offset offset,Size size,const void *inData)
{
	auto *dstPtr = static_cast<uint8_t*>(m_data.get()) +offset;
	memcpy(dstPtr,inData,size);
}
void util::ImageBuffer::Resize(Size width,Size height)
{
	// TODO
	throw std::runtime_error{"Resizing images not yet implemented!"};
}
#pragma optimize("",on)
