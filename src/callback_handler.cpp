// SPDX-FileCopyrightText: (c) 2019 Silverlan <opensource@pragma-engine.com>
// SPDX-License-Identifier: MIT

#include "sharedutils/callback_handler.h"
#include "sharedutils/callback_handler_impl.h"
#include <algorithm>
#include <assert.h>
#include "sharedutils/util_string.h"

std::string CallbackHandler::TranslateIdentifier(const std::string &identifier) const
{
	auto r = identifier;
	ustring::to_lower(r);
	return r;
}
