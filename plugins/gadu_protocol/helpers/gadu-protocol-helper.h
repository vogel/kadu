/*
 * %kadu copyright begin%
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2009 Bartłomiej Zimoń (uzi18@o2.pl)
 * Copyright 2004 Adrian Smarzewski (adrian@kadu.net)
 * Copyright 2007, 2008, 2009, 2010, 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2004, 2006 Marcin Ślusarz (joi@kadu.net)
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

#ifndef GADU_PROTOCOL_HELPER_H
#define GADU_PROTOCOL_HELPER_H

#include "status/status-type.h"

#include "gadu-exports.h"

#include "gadu-protocol.h"

namespace GaduProtocolHelper
{
	GADUAPI StatusType statusTypeFromGaduStatus(unsigned int index);
	GADUAPI bool isBlockingStatus(unsigned int index);
	GADUAPI unsigned int gaduStatusFromStatus(const Status &status);

	GADUAPI QString connectionErrorMessage(GaduProtocol::GaduError error);
	GADUAPI bool isConnectionErrorFatal(GaduProtocol::GaduError error);

	GADUAPI Buddy searchResultToBuddy(Account account, gg_pubdir50_t res, int number);

	GADUAPI UinType uin(Contact contact);
	GADUAPI GaduContactDetails * gaduContactDetails(Contact contact);
}

#endif // GADU_PROTOCOL_HELPER_H
