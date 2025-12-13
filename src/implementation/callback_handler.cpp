// SPDX-FileCopyrightText: (c) 2019 Silverlan <opensource@pragma-engine.com>
// SPDX-License-Identifier: MIT

module;

module pragma.util;

import :callback_handler;

std::string pragma::util::CallbackHandler::TranslateIdentifier(const std::string &identifier) const
{
	auto r = identifier;
	string::to_lower(r);
	return r;
}
