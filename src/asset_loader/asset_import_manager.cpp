// SPDX-FileCopyrightText: (c) 2021 Silverlan <opensource@pragma-engine.com>
// SPDX-License-Identifier: MIT

#include "sharedutils/asset_loader/file_asset_manager.hpp"

void util::FileAssetManager::RegisterImportHandler(const std::string &ext, const std::function<std::unique_ptr<util::IImportAssetFormatHandler>(util::IAssetManager &)> &factory, util::AssetFormatType formatType)
{
	m_importHandlers[ext] = factory;
	RegisterFileExtension(ext, formatType, FormatExtensionInfo::Type::Import);
}
bool util::FileAssetManager::Import(const std::string &path) { return Import(path, path); }
bool util::FileAssetManager::Import(const std::string &path, const std::string &outputPath)
{
	std::string ext;
	if(!ufile::get_extension(path, &ext))
		return false;
	return Import(path, outputPath, ext);
}
bool util::FileAssetManager::Import(const std::string &path, const std::string &outputPath, const std::string &ext, std::string *optOutErrMsg)
{
	auto itExt = FindExtension(m_extensions, ext, FormatExtensionInfo::Type::Import);
	if(itExt == m_extensions.end())
		return false;
	auto &extInfo = *itExt;
	auto cacheIdentifier = ToCacheIdentifier(path);
	auto extPath = cacheIdentifier + '.' + extInfo.extension;
	auto f = m_fileHandler->open(extPath, extInfo.formatType);
	if(!f)
		return false;
	auto it = m_importHandlers.find(ext);
	if(it == m_importHandlers.end())
		return false;
	auto &handlerFactory = it->second;
	auto handler = handlerFactory(*this);
	if(!handler)
		return false;
	handler->SetFile(std::move(f));
	auto normalizedOutputPath = m_importRootDir + ToCacheIdentifier(outputPath);
	std::string finalFilePath;
	auto res = handler->Import(normalizedOutputPath.GetString(), finalFilePath);
	if(!res) {
		if(optOutErrMsg) {
			auto &errMsg = handler->GetErrorMessage();
			if(errMsg)
				*optOutErrMsg = *errMsg;
			else
				*optOutErrMsg = "Unknown error";
		}
		return false;
	}
	return res;
}
