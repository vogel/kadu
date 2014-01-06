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

#ifndef PROTOCOL_GADU_CONNECTION_H
#define PROTOCOL_GADU_CONNECTION_H

#include <QtCore/QPointer>

#include "server/gadu-connection.h"

class GaduProtocol;

/**
 * @addtogroup Gadu
 * @{
 */

/**
 * @class ProtocolGaduConnection
 * @short Connection to Gadu Gadu server based on GaduProtocol instance.
 * @author Rafał 'Vogel' Malinowski
 *
 * This implementation of GaduConnection interface uses GaduProtocol for access to libgadu session
 * and to allow writing to it.
 */
class ProtocolGaduConnection : public GaduConnection
{
	Q_OBJECT

	QPointer<GaduProtocol> ConnectionProtocol;

protected:
	virtual gg_session * rawSession();
	virtual bool beginWrite();
	virtual bool endWrite();

public:
	/**
	 * @short Create new instance of ProtocolGaduConnection class.
	 * @author Rafał 'Vogel' Malinowski
	 * @param parent QObject parent
	 */
	explicit ProtocolGaduConnection(QObject *parent = 0);
	virtual ~ProtocolGaduConnection();

	/**
	 * @short Set GaduProtocol to use by this object.
	 * @author Rafał 'Vogel' Malinowski
	 * @param protocol GaduProtocol to use by this object
	 */
	void setConnectionProtocol(GaduProtocol *protocol);

	virtual bool hasSession();
	virtual GaduWritableSessionToken writableSessionToken();

};

/**
 * @}
 */

#endif // PROTOCOL_GADU_CONNECTION_H
