/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

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

	QList<StatusChanger *> StatusChangers;
	QMap<StatusContainer *, Status> LastStatuses;
	bool Enabled;

	StatusChangerManager();
	virtual ~StatusChangerManager();

public:
	static StatusChangerManager * instance();

	void enable();

	void registerStatusChanger(StatusChanger *statusChanger);
	void unregisterStatusChanger(StatusChanger *statusChanger);

	Status status(StatusContainer *statusContainer);

public slots:
	void statusChanged(StatusContainer *container = 0);

signals:
	void statusChanged(StatusContainer *container, Status status);

};

#include "accounts/account.h" // for MOC

#endif // STATUS_CHANGER_MANAGER_H
