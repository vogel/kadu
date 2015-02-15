/*
 * %kadu copyright begin%
 * Copyright 2009, 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2010 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2010, 2011 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2009, 2010, 2011 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#ifndef STATUS_TYPE_MANAGER
#define STATUS_TYPE_MANAGER

#include <QtCore/QList>
#include <QtCore/QMap>

#include "status/status-type-group.h"
#include "status/status-type.h"

#include "exports.h"

class QString;

class KaduIcon;
class Status;
class StatusTypeData;

/**
 * @addtogroup StatusTypeManager
 * @{
 */

/**
 * @class StatusTypeManager
 * @author Rafał 'Vogel' Malinowski
 * @short Manager containing StatusTypeData instances for each StatusType enum value.
 * @see StatusType
 * @see StatusTypeData
 *
 * This singleton class contains instances of StatusTypeData for each value of StatusType enum.
 */
class KADUAPI StatusTypeManager
{
	Q_DISABLE_COPY(StatusTypeManager)

	static StatusTypeManager *Instance;

	QMap<StatusType, StatusTypeData> StatusTypes;

	StatusTypeManager();
	~StatusTypeManager();

public:
	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Returns StatusTypeManager singleton instance.
	 * @return StatusTypeManager singleton instance
	 *
	 * Returns StatusTypeManager singleton instance.
	 */
	static StatusTypeManager * instance();

	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Returns StatusType enum value with given name.
	 * @param name of StatusType enum to search for
	 * @return StatusType enum value with given name
	 *
	 * Returns StatusType enum value with given name. If no valid enum value is found, StatusTypeOffline
	 * is returned.
	 */
	StatusType fromName(const QString &name);

	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Returns StatusTypeData instance for given StatusType enum value.
	 * @param statusType StatusType enum value
	 * @return StatusTypeData instance for given StatusType enum value
	 *
	 * Returns StatusTypeData instance for given StatusType enum value. If no valid StatusTypeData instance
	 * is found, valud for StatusTypeOffline is returned.
	 */
	const StatusTypeData statusTypeData(const StatusType statusType);

	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Returns icon for given status in given protocol.
	 * @param protocol protocol name
	 * @param status status
	 * @return icon for given status in given protocol
	 *
	 * Returns icon for given status in given protocol. Icons can be differen per protocols, status type
	 * and status description.
	 */
	KaduIcon statusIcon(const QString &protocol, const Status &status);

};

/**
 * @addtogroup Status
 * @}
 */

#endif // STATUS_TYPE_MANAGER
