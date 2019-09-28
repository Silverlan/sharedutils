/* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef __UTIL_IMAGE_BUFFER_HPP__
#define __UTIL_IMAGE_BUFFER_HPP__

#include "utildefinitions.h"
#include <cinttypes>
#include <memory>

namespace util
{
	class DLLSHUTIL ImageBuffer
		: public std::enable_shared_from_this<ImageBuffer>
	{
	public:
		enum class Format : uint8_t
		{
			None = 0u,
			RGB8,
			RGBA8,
			RGB16,
			RGBA16,
			RGB32,
			RGBA32,
			Count,

			RGB_LDR = RGB8,
			RGBA_LDR = RGBA8,
			RGB_HDR = RGB16,
			RGBA_HDR = RGBA16,
			RGB_FLOAT = RGB32,
			RGBA_FLOAT = RGBA32
		};
		enum class Channel : uint8_t
		{
			Red = 0,
			Green,
			Blue,
			Alpha,

			Count,

			R = Red,
			G = Green,
			B = Blue,
			A = Alpha
		};
		using Offset = size_t;
		using Size = size_t;
		using PixelIndex = uint32_t;
		using LDRValue = uint8_t;
		using HDRValue = uint16_t;
		using FloatValue = float;
		class PixelIterator;
		struct DLLSHUTIL PixelView
		{
			Offset GetOffset() const;
			PixelIndex GetPixelIndex() const;
			uint32_t GetX() const;
			uint32_t GetY() const;
			const void *GetPixelData() const;
			void *GetPixelData();
			LDRValue GetLDRValue(Channel channel) const;
			HDRValue GetHDRValue(Channel channel) const;
			FloatValue GetFloatValue(Channel channel) const;
			void SetValue(Channel channel,LDRValue value);
			void SetValue(Channel channel,HDRValue value);
			void SetValue(Channel channel,FloatValue value);
			void CopyValues(const PixelView &outOther);
			void CopyValue(Channel channel,const PixelView &outOther);

			ImageBuffer &GetImageBuffer() const;
		private:
			PixelView(ImageBuffer &imgBuffer,Offset offset);
			friend PixelIterator;
			friend ImageBuffer;
			ImageBuffer &m_imageBuffer;
			Offset m_offset = 0u;
		};
		class DLLSHUTIL PixelIterator
		{
		public:
			using iterator_category = std::forward_iterator_tag;
			using value_type = PixelView;
			using difference_type = PixelView;
			using pointer = PixelView*;
			using reference = PixelView&;

			PixelIterator &operator++();
			PixelIterator operator++(int);
			PixelView &operator*();
			PixelView *operator->();
			bool operator==(const PixelIterator &other) const;
			bool operator!=(const PixelIterator &other) const;
		private:
			friend ImageBuffer;
			PixelIterator(ImageBuffer &imgBuffer,Offset offset);
			PixelView m_pixelView;
		};

		static std::shared_ptr<ImageBuffer> Create(void *data,uint32_t width,uint32_t height,Format format,bool ownedExternally=true);
		static std::shared_ptr<ImageBuffer> Create(const void *data,uint32_t width,uint32_t height,Format format);
		static std::shared_ptr<ImageBuffer> Create(uint32_t width,uint32_t height,Format format);
		static Size GetPixelSize(Format format);
		static uint8_t GetChannelSize(Format format);
		static uint8_t GetChannelCount(Format format);
		static LDRValue ToLDRValue(HDRValue value);
		static LDRValue ToLDRValue(FloatValue value);
		static HDRValue ToHDRValue(LDRValue value);
		static HDRValue ToHDRValue(FloatValue value);
		static FloatValue ToFloatValue(LDRValue value);
		static FloatValue ToFloatValue(HDRValue value);

		ImageBuffer(const ImageBuffer&)=default;
		ImageBuffer &operator=(const ImageBuffer&)=default;
		Format GetFormat() const;
		uint32_t GetWidth() const;
		uint32_t GetHeight() const;
		uint8_t GetChannelCount() const;
		uint8_t GetChannelSize() const;
		Size GetPixelSize() const;
		uint32_t GetPixelCount() const;
		bool HasAlphaChannel() const;
		bool IsLDRFormat() const;
		bool IsHDRFormat() const;
		bool IsFloatFormat() const;
		const void *GetData() const;
		void *GetData();
		std::shared_ptr<ImageBuffer> Copy() const;
		std::shared_ptr<ImageBuffer> Copy(Format format) const;
		void Convert(Format targetFormat);
		void ToLDR();
		void ToHDR();
		void ToFloat();
		Size GetSize() const;

		PixelIndex GetPixelIndex(uint32_t x,uint32_t y) const;
		Offset GetPixelOffset(uint32_t x,uint32_t y) const;

		void Read(Offset offset,Size size,void *outData);
		void Write(Offset offset,Size size,const void *inData);
		void Resize(Size width,Size height);

		void FlipHorizontally();
		void FlipVertically();

		void InitPixelView(uint32_t x,uint32_t y,PixelView &pxView);
		PixelView GetPixelView();

		PixelIterator begin();
		PixelIterator end();
	private:
		static void Convert(ImageBuffer &srcImg,ImageBuffer &dstImg,Format targetFormat);
		ImageBuffer(const std::shared_ptr<void> &data,uint32_t width,uint32_t height,Format format);
		void Reallocate();
		std::shared_ptr<void> m_data = nullptr;
		uint32_t m_width = 0u;
		uint32_t m_height = 0u;
		Format m_format = Format::None;
	};
};

#endif
