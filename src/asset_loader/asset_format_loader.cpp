/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "sharedutils/asset_loader/asset_format_loader.hpp"
#include "sharedutils/asset_loader/asset_format_handler.hpp"

#undef AddJob

util::AssetFormatLoader::AssetFormatLoader(util::IAssetManager &assetManager, std::string name) : IAssetLoader {std::move(name)}, m_assetManager {assetManager} {}
void util::AssetFormatLoader::RegisterFormatHandler(const std::string &ext, const std::function<std::unique_ptr<IAssetFormatHandler>(util::IAssetManager &)> &factory) { m_formatHandlers[ext] = factory; }

std::optional<util::AssetLoadJobId> util::AssetFormatLoader::AddJob(const std::string &identifier, const std::string &ext, std::unique_ptr<ufile::IFile> &&file, util::AssetLoadJobPriority priority, const std::function<void(util::IAssetProcessor &)> &initProcessor)
{
	auto it = m_formatHandlers.find(ext);
	if(it == m_formatHandlers.end())
		return {};
	auto handler = it->second(m_assetManager);
	if(!handler)
		return {};
	handler->SetFile(std::move(file));
	auto processor = CreateAssetProcessor(identifier, ext, std::move(handler));
	if(initProcessor)
		initProcessor(*processor);
	auto jobId = IAssetLoader::AddJob(identifier, std::move(processor), priority);
	if(!jobId.has_value())
		file = handler->GetFile();
	return jobId;
}
