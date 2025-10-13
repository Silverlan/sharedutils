// SPDX-FileCopyrightText: (c) 2021 Silverlan <opensource@pragma-engine.com>
// SPDX-License-Identifier: MIT

module;

#include "sharedutils/utildefinitions.h"
#include <cinttypes>
#include <optional>
#include <string>
#include <functional>
#include <memory>

export module pragma.util:asset_loader.processor;

export namespace util {
	class DLLSHUTIL IAssetProcessor {
	  public:
		virtual ~IAssetProcessor() = default;
		virtual bool Load() = 0;
		virtual bool Finalize() = 0;
		virtual std::optional<std::string> GetErrorMessage() const = 0;
		virtual void Close() {};
	};
};
