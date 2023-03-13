/* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef __ASSET_FORMAT_HANDLER_HPP__
#define __ASSET_FORMAT_HANDLER_HPP__

#include "sharedutils/utildefinitions.h"
#include "sharedutils/util_ifile.hpp"

namespace util {
	class IAssetManager;
	class DLLSHUTIL IAssetFormatHandler {
	  public:
		virtual ~IAssetFormatHandler() = default;
		void SetFile(std::unique_ptr<ufile::IFile> &&file) { m_file = std::move(file); }
		std::unique_ptr<ufile::IFile> GetFile() { return std::move(m_file); }
		void Close() { m_file = nullptr; }
		util::IAssetManager &GetAssetManager() { return m_assetManager; }
		const util::IAssetManager &GetAssetManager() const { return const_cast<IAssetFormatHandler *>(this)->GetAssetManager(); }
		const std::optional<std::string> &GetErrorMessage() const { return m_error; }
	  protected:
		IAssetFormatHandler(util::IAssetManager &assetManager);
		std::unique_ptr<ufile::IFile> m_file = nullptr;
		std::optional<std::string> m_error {};
		util::IAssetManager &m_assetManager;
	};

	class DLLSHUTIL IImportAssetFormatHandler : public IAssetFormatHandler {
	  public:
		IImportAssetFormatHandler(util::IAssetManager &assetManager) : IAssetFormatHandler {assetManager} {}
		virtual bool Import(const std::string &outputPath, std::string &outFilePath) = 0;
	};
};

#endif
