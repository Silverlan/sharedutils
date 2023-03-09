/* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef __ASSET_LOAD_JOB_HPP__
#define __ASSET_LOAD_JOB_HPP__

#include "sharedutils/utildefinitions.h"
#include <cinttypes>
#include <string>
#include <memory>
#include <chrono>

namespace util {
	using AssetLoadJobId = uint64_t;
	using AssetLoadJobPriority = int32_t;
	class ITextureFormatHandler;
	class TextureProcessor;
	class IAssetProcessor;
	struct DLLSHUTIL AssetLoadJob {
		enum class State : uint8_t { Pending = 0, Succeeded, Failed, Cancelled };
		AssetLoadJob() = default;
		AssetLoadJob(AssetLoadJob &&other) = default;
		AssetLoadJob(const AssetLoadJob &other) = default;
		AssetLoadJob &operator=(const AssetLoadJob &) = default;
		AssetLoadJob &operator=(AssetLoadJob &&) = default;

		std::shared_ptr<IAssetProcessor> processor = nullptr;
		State state = State::Pending;
		AssetLoadJobPriority priority = 0;
		std::string identifier;
		AssetLoadJobId jobId = 0;

		std::chrono::high_resolution_clock::time_point queueStartTime;
		std::chrono::high_resolution_clock::time_point completionTime;
		std::chrono::high_resolution_clock::time_point taskStartTime;
	};
	class DLLSHUTIL CompareTextureLoadJob {
	  public:
		bool operator()(AssetLoadJob &t1, AssetLoadJob &t2) { return t1.priority > t2.priority; }
	};
};

#endif
