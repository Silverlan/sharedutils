/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "sharedutils/asset_loader/file_asset_manager.hpp"

void util::FileAssetManager::RegisterImportHandler(
	const std::string &ext,const std::function<std::unique_ptr<util::IImportAssetFormatHandler>(util::IAssetManager&)> &factory,
	util::AssetFormatType formatType
)
{
	m_importHandlers[ext] = factory;
	RegisterFileExtension(ext,formatType,FormatExtensionInfo::Type::Import);
}
bool util::FileAssetManager::Import(const std::string &path)
{
	std::string ext;
	if(!ufile::get_extension(path,&ext))
		return false;
	return Import(path,ext);
}
bool util::FileAssetManager::Import(const std::string &path,const std::string &ext)
{
	auto itExt = FindExtension(m_extensions,ext,FormatExtensionInfo::Type::Import);
	if(itExt == m_extensions.end())
		return false;
	auto &extInfo = *itExt;
	auto cacheIdentifier = ToCacheIdentifier(path);
	auto extPath = cacheIdentifier +'.' +extInfo.extension;
	auto f = m_fileHandler->open((m_rootDir +extPath).GetString(),extInfo.formatType);
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
	auto outputPath = m_importRootDir +cacheIdentifier;
	std::string finalFilePath;
	return handler->Import(outputPath.GetString(),finalFilePath);
}
