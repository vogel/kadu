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

#ifndef GADU_CONNECTION_H
#define GADU_CONNECTION_H

#include <QtCore/QObject>

struct gg_session;
class GaduWritableSessionToken;

/**
 * @addtogroup Gadu
 * @{
 */

/**
 * @class GaduConnection
 * @short Interface descibing connection to Gadu Gadu server.
 * @author Rafał 'Vogel' Malinowski
 *
 * This interface allows for access to raw libgadu session. It also contains two methods that must be called
 * before and after each write using this session.
 */
class GaduConnection : public QObject
{
	Q_OBJECT

	friend class GaduWritableSessionToken;

protected:
	explicit GaduConnection(QObject *parent = 0) : QObject(parent) {}
	virtual ~GaduConnection() {}

	/**
	 * @short Return current libgadu session.
	 * @author Rafał 'Vogel' Malinowski
	 * @return libgadu session
	 */
	virtual gg_session * rawSession() = 0;

	/**
	 * @short Call before writing to connection session.
	 * @author Rafał 'Vogel' Malinowski
	 * @return true if write can be performed
	 */
	virtual bool beginWrite() = 0;

	/**
	 * @short Call after writing to connection session.
	 * @author Rafał 'Vogel' Malinowski
	 * @return true if operation succeeded
	 */
	virtual bool endWrite() = 0;

public:
	/**
	 * @short Return true if connection is valid and has session.
	 * @author Rafał 'Vogel' Malinowski
	 * @return true if connection is valid and has session
	 */
	virtual bool hasSession() = 0;

	/**
	 * @short Return instance of writable session.
	 * @author Rafał 'Vogel' Malinowski
	 * @return libgadu session
	 *
	 * It is safe to send any message to session until object is valid. Sockets will be
	 * handled properly in meantime.
	 */
	virtual GaduWritableSessionToken writableSessionToken() = 0;

};

/**
 * @}
 */

#endif // GADU_CONNECTION_H
