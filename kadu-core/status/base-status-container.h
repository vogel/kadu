/*
 * %kadu copyright begin%
 * Copyright 2009 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009, 2010, 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2009 Michał Podsiadlik (michal@kadu.net)
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

#ifndef BASE_STATUS_CONTAINER_H
#define BASE_STATUS_CONTAINER_H

#include "status/status-container.h"

#include "exports.h"

class StorableObject;

class KADUAPI BaseStatusContainer : public StatusContainer
{
	Q_OBJECT

	StorableObject *MyStorableObject;

protected:
	virtual void doSetStatus(Status status) = 0;

public:
	explicit BaseStatusContainer(StorableObject *storableObject);
	virtual ~BaseStatusContainer();

	virtual void setStatus(Status status);
	virtual void setDescription(const QString &description);

	virtual void setDefaultStatus(const QString &startupStatus, bool offlineToInvisible,
			const QString &startupDescription, bool StartupLastDescription);
	virtual void storeStatus(Status status);

};

#endif // BASE_STATUS_CONTAINER_H
