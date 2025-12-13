// SPDX-FileCopyrightText: (c) 2021 Silverlan <opensource@pragma-engine.com>
// SPDX-License-Identifier: MIT

module;

#include "definitions.hpp"

export module pragma.util:asset_loader.format_handler;

export import :file_interface;

export namespace pragma::util {
	class IAssetManager;
	class DLLSHUTIL IAssetFormatHandler {
	  public:
		virtual ~IAssetFormatHandler() = default;
		void SetFile(std::unique_ptr<ufile::IFile> &&file) { m_file = std::move(file); }
		std::unique_ptr<ufile::IFile> GetFile() { return std::move(m_file); }
		void Close() { m_file = nullptr; }
		pragma::util::IAssetManager &GetAssetManager() { return m_assetManager; }
		const pragma::util::IAssetManager &GetAssetManager() const { return const_cast<IAssetFormatHandler *>(this)->GetAssetManager(); }
		const std::optional<std::string> &GetErrorMessage() const { return m_error; }
	  protected:
		IAssetFormatHandler(pragma::util::IAssetManager &assetManager);
		std::unique_ptr<ufile::IFile> m_file = nullptr;
		std::optional<std::string> m_error {};
		pragma::util::IAssetManager &m_assetManager;
	};

	class DLLSHUTIL IImportAssetFormatHandler : public IAssetFormatHandler {
	  public:
		IImportAssetFormatHandler(pragma::util::IAssetManager &assetManager) : IAssetFormatHandler {assetManager} {}
		virtual bool Import(const std::string &outputPath, std::string &outFilePath) = 0;
	};
};
