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

#include "configuration/configuration-aware-object.h"
#include "status/status.h"
#include "status/status-container-aware-object.h"

class Account;
class StatusChanger;
class StatusContainer;

class KADUAPI StatusChangerManager : public QObject, private StatusContainerAwareObject, private ConfigurationAwareObject
{
	Q_OBJECT
	Q_DISABLE_COPY(StatusChangerManager)

	static StatusChangerManager *Instance;

	bool CoreInitialized;
	QString StartupStatus;
	QString StartupDescription;
	bool StartupLastDescription;
	bool OfflineToInvisible;

	QMap<StatusContainer *, Status> Statuses;
	QList<StatusChanger *> StatusChangers;

	StatusChangerManager();
	virtual ~StatusChangerManager();

	void setDefaultStatus(StatusContainer *statusContainer);

private slots:
	void statusChanged(StatusContainer *statusContainer = 0);

protected:
	void configurationUpdated();

	void statusContainerRegistered(StatusContainer *statusContainer);
	void statusContainerUnregistered(StatusContainer *statusContainer);

public:
	static StatusChangerManager * instance();

	void coreInitialized();

	void registerStatusChanger(StatusChanger *statusChanger);
	void unregisterStatusChanger(StatusChanger *statusChanger);

	void setStatus(StatusContainer *statusContainer, Status status);
	Status manuallySetStatus(StatusContainer *statusContainer);

};

#include "accounts/account.h" // for MOC

#endif // STATUS_CHANGER_MANAGER_H
