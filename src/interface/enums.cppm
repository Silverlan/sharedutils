// SPDX-FileCopyrightText: (c) 2020 Silverlan <opensource@pragma-engine.com>
// SPDX-License-Identifier: MIT

module;

export module pragma.util:enums;

export import std.compat;

export {
	namespace pragma::util {
		enum class EventReply : uint8_t { Unhandled = 0, Handled };
	};

	enum class AlphaMode : uint32_t {
		Opaque = 0,
		Mask,
		Blend,

		Count
	};
}
