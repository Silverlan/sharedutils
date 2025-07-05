// SPDX-FileCopyrightText: (c) 2021 Silverlan <opensource@pragma-engine.com>
// SPDX-License-Identifier: MIT

#ifndef __ASSET_PROCESSOR_HPP__
#define __ASSET_PROCESSOR_HPP__

#include "sharedutils/utildefinitions.h"
#include <cinttypes>
#include <optional>
#include <string>
#include <functional>
#include <memory>

namespace prosper {
	class IBuffer;
	class IPrContext;
	class IImage;
};
namespace uimg {
	class ImageBuffer;
};
namespace util {
	class ITextureFormatHandler;
	class DLLSHUTIL IAssetProcessor {
	  public:
		virtual ~IAssetProcessor() = default;
		virtual bool Load() = 0;
		virtual bool Finalize() = 0;
		virtual std::optional<std::string> GetErrorMessage() const = 0;
		virtual void Close() {};
	};
};

#endif
