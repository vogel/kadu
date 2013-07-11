/*
 * %kadu copyright begin%
 * Copyright 2013 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * %kadu copyright end%
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include "otr-trust-level.h"

OtrTrustLevel::Level OtrTrustLevel::fromContext(ConnContext *context)
{
	if (!context)
		return TRUST_NOT_PRIVATE;

	if (context->msgstate == OTRL_MSGSTATE_FINISHED)
		return TRUST_NOT_PRIVATE;

	if (context->msgstate != OTRL_MSGSTATE_ENCRYPTED)
		return TRUST_NOT_PRIVATE;

	if (!context->active_fingerprint)
		return TRUST_UNVERIFIED;

	if (!context->active_fingerprint->trust)
		return TRUST_UNVERIFIED;

	if (context->active_fingerprint->trust[0] == '\0')
		return TRUST_UNVERIFIED;

	return TRUST_PRIVATE;
}
