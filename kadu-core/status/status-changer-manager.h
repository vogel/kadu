/*
 * %kadu copyright begin%
 * Copyright 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2010 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#ifndef STATUS_CHANGER_MANAGER_H
#define STATUS_CHANGER_MANAGER_H

#include <QtCore/QMap>

#include "status/status.h"

class Account;
class StatusChanger;
class StatusContainer;

class KADUAPI StatusChangerManager : public QObject
{
	Q_OBJECT
	Q_DISABLE_COPY(StatusChangerManager)

	static StatusChangerManager *Instance;

	QMap<StatusContainer *, Status> Statuses;
	QMap<StatusContainer *, Status> RealStatuses;
	QList<StatusChanger *> StatusChangers;

	StatusChangerManager();
	virtual ~StatusChangerManager();

private slots:
	void statusChanged(StatusContainer *container = 0);

public:
	static StatusChangerManager * instance();

	void registerStatusChanger(StatusChanger *statusChanger);
	void unregisterStatusChanger(StatusChanger *statusChanger);

	void setStatus(StatusContainer *statusContainer, Status status);
	Status realStatus(StatusContainer *statusContainer);
	Status manuallySetStatus(StatusContainer *statusContainer);

signals:
	void statusChanged(StatusContainer *container, Status status);

};

#include "accounts/account.h" // for MOC

#endif // STATUS_CHANGER_MANAGER_H
