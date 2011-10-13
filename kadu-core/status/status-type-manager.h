/*
 * %kadu copyright begin%
 * Copyright 2010 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2010 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2009, 2010 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "status/status-type.h"
#include "status/status-type-group.h"

#include "exports.h"

class QString;

class KaduIcon;
class Status;
class StatusTypeData;

class KADUAPI StatusTypeManager
{
	Q_DISABLE_COPY(StatusTypeManager)

	static StatusTypeManager *Instance;

	QMap<StatusType, StatusTypeData> StatusTypes;

	StatusTypeManager();
	~StatusTypeManager();

public:
	static StatusTypeManager * instance();

	StatusType fromName(const QString &name);
	const StatusTypeData statusTypeData(const StatusType statusType);

	KaduIcon statusIcon(const QString &protocol, const Status &status);

};

#endif // STATUS_TYPE_MANAGER
