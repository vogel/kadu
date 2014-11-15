/*
 * %kadu copyright begin%
 * Copyright 2014 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#pragma once

#include "protocols/services/account-service.h"

#include <QtCore/QPointer>

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
class KADUAPI ProtocolService : public AccountService
{
	Q_OBJECT

public:
	/**
	 * @short Returns protocol bounded with this service.
	 * @author Rafał 'Vogel' Malinowski
	 * @return protocol bounded with this service
	 */
	Protocol * protocol() const;

protected:
	explicit ProtocolService(Protocol *protocol, QObject *parent = nullptr);
	virtual ~ProtocolService();

private:
	QPointer<Protocol> m_protocol;

};

/**
 * @}
 */
