// SPDX-FileCopyrightText: (c) 2021 Silverlan <opensource@pragma-engine.com>
// SPDX-License-Identifier: MIT

module;

#include "definitions.hpp"


export module pragma.util:file_interface;

export import std.compat;

export namespace ufile {
	struct DLLSHUTIL IFile {
		template<typename T>
		T Read()
		{
			T v;
			Read(&v, sizeof(T));
			return v;
		}
		template<typename T>
		size_t Write(const T &v)
		{
			return Write(&v, sizeof(T));
		}
		enum class Whence : uint8_t { Set = 0, Cur, End };
		virtual ~IFile() = default;
		virtual size_t Read(void *data, size_t size) = 0;
		virtual size_t Write(const void *data, size_t size) = 0;
		virtual size_t Tell() = 0;
		virtual void Seek(size_t offset, Whence whence = Whence::Set) = 0;
		virtual int32_t ReadChar() = 0;
		virtual size_t GetSize()
		{
			auto pos = Tell();
			Seek(0, Whence::End);
			auto size = Tell();
			Seek(pos);
			return size;
		}
		virtual bool Eof() { return Tell() >= GetSize(); }
		virtual std::optional<std::string> GetFileName() const { return {}; }

		int32_t WriteString(const std::string &str);
		std::string ReadString();
		std::string ReadLine();
	};

	struct DLLSHUTIL MemoryFile : public ufile::IFile {
		MemoryFile(uint8_t *data, size_t dataSize);
		void *GetData() { return m_data; }
		const void *GetData() const { return const_cast<MemoryFile *>(this)->GetData(); }
		size_t GetDataSize() const { return m_dataSize; }
		virtual size_t Read(void *data, size_t size) override;
		virtual size_t Write(const void *data, size_t size) override;
		virtual size_t Tell() override;
		virtual void Seek(size_t offset, Whence whence = Whence::Set) override;
		virtual int32_t ReadChar() override;
		virtual size_t GetSize() override { return m_dataSize; }
		char *GetMemoryDataPtr();

		template<typename T>
		T &GetValue()
		{
			return *reinterpret_cast<T *>(GetMemoryDataPtr());
		}
		template<typename T>
		const T &GetValue() const
		{
			return const_cast<MemoryFile *>(this)->GetValue<T>();
		}

		template<typename T>
		T &GetValueAndAdvance()
		{
			auto &val = GetValue<T>();
			Seek(sizeof(T), Whence::Cur);
			return val;
		}
		template<typename T>
		const T &GetValueAndAdvance() const
		{
			return const_cast<MemoryFile *>(this)->GetValueAndAdvance<T>();
		}
	  protected:
		uint8_t *m_data = nullptr;
		size_t m_dataSize = 0;
		size_t m_pos = 0;
	};

	struct DLLSHUTIL VectorFile : public MemoryFile {
		VectorFile();
		VectorFile(size_t size);
		VectorFile(std::vector<uint8_t> &&data);
		const std::vector<uint8_t> &GetVector() const;
		void Resize(size_t size);
		VectorFile &operator=(std::vector<uint8_t> &&data);
		void Move(std::vector<uint8_t> &data);
	  private:
		std::vector<uint8_t> m_data;
	};

	struct DLLSHUTIL BaseStreamFile : public ufile::IFile {
		std::stringstream MoveStream();
	  protected:
		BaseStreamFile(std::ios_base::openmode openMode);
		BaseStreamFile(std::stringstream &&stream);
		std::stringstream m_stream;
	};

	struct DLLSHUTIL InStreamFile : public BaseStreamFile {
		InStreamFile();
		InStreamFile(std::stringstream &&stream);
		virtual size_t Read(void *data, size_t size) override;
		virtual size_t Write(const void *data, size_t size) override;
		virtual size_t Tell() override;
		virtual void Seek(size_t offset, Whence whence = Whence::Set) override;
		virtual int32_t ReadChar() override;
		virtual size_t GetSize() override;
	};

	struct DLLSHUTIL OutStreamFile : public BaseStreamFile {
		OutStreamFile();
		OutStreamFile(std::stringstream &&stream);
		virtual size_t Read(void *data, size_t size) override;
		virtual size_t Write(const void *data, size_t size) override;
		virtual size_t Tell() override;
		virtual void Seek(size_t offset, Whence whence = Whence::Set) override;
		virtual int32_t ReadChar() override;
		virtual size_t GetSize() override;
	};

	struct DLLSHUTIL FileWrapper : public ufile::IFile {
		FileWrapper();
		FileWrapper(const std::shared_ptr<IFile> &f);
		virtual size_t Read(void *data, size_t size) override;
		virtual size_t Write(const void *data, size_t size) override;
		virtual size_t Tell() override;
		virtual void Seek(size_t offset, Whence whence = Whence::Set) override;
		virtual int32_t ReadChar() override;
		virtual size_t GetSize() override;
		virtual bool Eof() override;
		virtual std::optional<std::string> GetFileName() const override;
	  private:
		std::shared_ptr<IFile> m_file;
	};
};
