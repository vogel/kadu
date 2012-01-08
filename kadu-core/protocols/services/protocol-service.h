/*
 * %kadu copyright begin%
 * Copyright 2012 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#ifndef PROTOCOL_SERVICE_H
#define PROTOCOL_SERVICE_H

#include <QtCore/QObject>

#include "buddies/buddy.h"
#include "contacts/contact.h"

#include "exports.h"

class Account;
class Protocol;

/**
 * @addtogroup Protocol
 * @{
 */

/**
 * @class ProtocolService
 * @author Rafał 'Vogel' Malinowski
 * @short Generic protocol service.
 *
 * All protocol services should derive from this class.
 */
class KADUAPI ProtocolService : public QObject
{
	Q_OBJECT

	Protocol *CurrentProtocol;

public:
	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Create new instance of ProtocolService bound to given Protocol.
	 * @param protocol protocol to bound this service to
	 */
	explicit ProtocolService(Protocol *protocol);
	virtual ~ProtocolService();

	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Returns protocol bounded with this service.
	 * @return protocol bounded with this service
	 */
	Protocol * protocol() const { return CurrentProtocol; }

	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Returns account bounded with this service.
	 * @return account bounded with this service
	 *
	 * This is shortcut to calling protocol().account().
	 */
	Account account() const;
};

/**
 * @}
 */

#endif // PROTOCOL_SERVICE_H
