// SPDX-FileCopyrightText: (c) 2026 Silverlan <opensource@pragma-engine.com>
// SPDX-License-Identifier: MIT

export module pragma.util:global_string_table;

export import std.compat;

export {
	namespace pragma::util {
		struct DLLSHUTIL GlobalStringTable {
			const char *RegisterString(const std::string &str);
			const char *RegisterString(const std::string_view &str);
			const char *RegisterString(const char *str);
			~GlobalStringTable();
		  private:
			std::unordered_map<std::string, const char *> m_strings;
			std::mutex m_mutex;
		};

#pragma pack(push, 1)
		struct DLLSHUTIL GString {
			GString();
			GString(const char *str);
			GString(const std::string &str);
			GString(const std::string_view &str);
			GString(const GString &other);
			GString &operator=(const char *str);
			GString &operator=(const std::string &str);
			GString &operator=(const std::string_view &str);
			GString &operator=(const GString &str);
			bool operator==(const char *str) const;
			bool operator!=(const char *str) const;
			bool operator==(const std::string &str) const;
			bool operator!=(const std::string &str) const;
			bool operator==(const std::string_view &str) const;
			bool operator!=(const std::string_view &str) const;
			bool operator==(const GString &str) const;
			bool operator!=(const GString &str) const;
			explicit operator const char *() const;
			operator std::string() const;
			operator std::string_view() const;
			explicit operator bool() const;
			const char *operator*() const;
			const char *c_str() const;
			bool empty() const;
			size_t length() const;
			const char *str = nullptr;
		};
#pragma pack(pop)
		DLLSHUTIL void set_global_string_table(GlobalStringTable &table);
		DLLSHUTIL GlobalStringTable &get_global_string_table();
		DLLSHUTIL const char *register_global_string(const std::string &str);
		DLLSHUTIL const char *register_global_string(const std::string_view &str);
		DLLSHUTIL const char *register_global_string(const char *str);
		DLLSHUTIL std::ostream &operator<<(std::ostream &stream, const GString &str);
	};

	// This causes unresolved externals when building with clang on Windows
#ifndef WINDOWS_CLANG_COMPILER_FIX
	template<>
	struct std::hash<pragma::util::GString> {
		std::size_t operator()(const pragma::util::GString &k) const { return std::hash<std::string>()(k.c_str()); }
	};
#endif
};
