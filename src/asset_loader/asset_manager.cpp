/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "sharedutils/asset_loader/asset_manager.hpp"
#include "sharedutils/util_path.hpp"
#include <cctype>
#pragma optimize("",off)
static bool contains_dot_file(const std::string_view &key,size_t i)
{
	auto c = key[i];
	if(c == '.')
	{
		if(i == 0 || key[i -1] == '/' || key[i -1] == '\\')
		{
			// '*/.*' or '.*'
			if(i == key.length() -1)
			{
				// '*/.' or '.'
				return true;
			}
			else
			{
				if(key[i +1] == '/' || key[i +1] == '\\')
				{
					// '*/./*' or './*'
					return true;
				}
				else if(key[i +1] == '.')
				{
					// '*/..*' or '..*'
					if(i +1 == key.length() -1)
					{
						// '*/..' or '..'
						return true;
					}
					else if(key[i +2] == '/' || key[i +2] == '\\')
					{
						// '*/../*' or '../*'
						return true;
					}
				}
			}
		}
	}
	return false;
}

// Generates a fast hash for quick lookups
unsigned long hash_identifier(std::string_view key)
{
	// djb2 hash
	unsigned long hash = 5381;
	auto len = key.length();
	size_t offset = 0;
	for(auto i=offset;i<key.length();++i)
	{
		auto idx = i -offset;
		auto c = (unsigned char)key[i];
		if(c == '\\')
			c = '/';
		if(c == '/')
		{
			if(idx == 0)
				continue;
		}
		c = std::tolower(static_cast<unsigned char>(c));
		if(contains_dot_file(key,i))
		{
			// Path contains '.' or '..', we'll have to canonicalize and re-hash
			auto path = util::Path::CreateFile(std::string{key});
			path.Canonicalize();
			auto &strPath = path.GetString();
			return hash_identifier(strPath);
		}

		hash = 33 * hash + c;
	}
	return hash;
}

void util::IAssetManager::StripFileExtension(std::string_view &key) const
{
	if(key.empty())
		return;
	auto i = key.length() -1;
	while(i >= 0)
	{
		auto c = std::tolower(static_cast<unsigned char>(key[i]));
		if(c == '/' || c == '\\')
			return;
		if(c != '.')
		{
			--i;
			continue;
		}
		auto hash = hash_identifier(key.substr(i +1));
		auto it = std::find(m_extensionHashes.begin(),m_extensionHashes.end(),hash);
		if(it != m_extensionHashes.end())
			key = key.substr(0,i);
		return;
	}
}

util::IAssetManager::IAssetManager()
{}
void util::IAssetManager::AddToCache(const std::string &assetName,const std::shared_ptr<IAsset> &asset)
{
	auto identifier = ToCacheIdentifier(assetName);
	m_cache[GetIdentifierHash(identifier)] = AssetInfo{asset,assetName};
}
void util::IAssetManager::RegisterFileExtension(const std::string &ext) {m_extensionHashes.push_back(hash_identifier(ext));}
size_t util::IAssetManager::GetIdentifierHash(const std::string &assetName) const
{
	std::string_view strView {assetName};
	StripFileExtension(strView);
	return hash_identifier(strView);
}
std::string util::IAssetManager::ToCacheIdentifier(const std::string &assetName)
{
	auto path = util::Path::CreateFile(assetName);
	path.Canonicalize();
	std::string_view v {path.GetString()};
	StripFileExtension(v);
	return std::string{v};
}

uint32_t util::IAssetManager::ClearUnused()
{
	uint32_t n = 0;
	for(auto it=m_cache.begin();it!=m_cache.end();)
	{
		auto &assetInfo = it->second;
		if(assetInfo.asset->IsInUse() == false)
		{
			it = m_cache.erase(it);
			++n;
			continue;
		}
		++it;
	}
	return n;
}
uint32_t util::IAssetManager::ClearFlagged()
{
	uint32_t n = 0;
	for(auto &name : m_flaggedForDeletion)
	{
		auto itCache = m_cache.find(name);
		if(itCache == m_cache.end())
			continue;
		m_cache.erase(itCache);
		++n;
	}
	m_flaggedForDeletion.clear();
	return n;
}
uint32_t util::IAssetManager::Clear()
{
	auto n = m_cache.size();
	m_cache.clear();
	m_flaggedForDeletion.clear();
	return n;
}
void util::IAssetManager::FlagForRemoval(const std::string &assetName)
{
	auto hash = GetIdentifierHash(assetName);
	auto it = m_cache.find(hash);
	if(it == m_cache.end())
		return;
	m_flaggedForDeletion.insert(hash);
}

void util::IAssetManager::FlagAllForRemoval()
{
	m_flaggedForDeletion.reserve(m_cache.size());
	for(auto &pair : m_cache)
		m_flaggedForDeletion.insert(pair.first);
}

const std::unordered_map<size_t,util::IAssetManager::AssetInfo> &util::IAssetManager::GetCache() const {return m_cache;}

const util::IAsset *util::IAssetManager::FindCachedAsset(const std::string &assetName) const
{
	return const_cast<IAssetManager*>(this)->FindCachedAsset(assetName);
}
util::IAsset *util::IAssetManager::FindCachedAsset(const std::string &assetName)
{
	auto it = m_cache.find(GetIdentifierHash(assetName));
	return (it != m_cache.end()) ? it->second.asset.get() : nullptr;
}
#pragma optimize("",on)
