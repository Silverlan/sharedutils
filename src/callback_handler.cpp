/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

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
