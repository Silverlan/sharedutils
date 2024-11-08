/* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at http://mozilla.org/MPL/2.0/. */

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
