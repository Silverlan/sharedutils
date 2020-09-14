/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "sharedutils/util_hash.hpp"

uint32_t util::distribute(const uint32_t& n)
{
	uint32_t p = 0x55555555ul; // pattern of alternating 0 and 1
	uint32_t c = 3423571495ul; // random uneven integer constant; 
	return c*xorshift(p*xorshift(n,16),16);
}

uint64_t util::hash(const uint64_t& n)
{
	uint64_t p = 0x5555555555555555;     // pattern of alternating 0 and 1
	uint64_t c = 17316035218449499591ull;// random uneven integer constant; 
	return c*xorshift(p*xorshift(n,32),32);
}
