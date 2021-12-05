/* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef __ASSET_PROCESSOR_HPP__
#define __ASSET_PROCESSOR_HPP__

#include "sharedutils/utildefinitions.h"
#include <cinttypes>
#include <optional>
#include <functional>
#include <memory>

namespace prosper {class IBuffer; class IPrContext; class IImage;};
namespace uimg {class ImageBuffer;};
namespace util
{
	struct IAsset;
	class ITextureFormatHandler;
	struct DLLSHUTIL IAssetProcessor
	{
	public:
		virtual bool Load()=0;
		virtual bool Finalize()=0;
	};
};

#endif
