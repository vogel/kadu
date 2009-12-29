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

#include "status/status.h"

class StatusChanger;

class KADUAPI StatusChangerManager : public QObject
{
	Q_OBJECT
	Q_DISABLE_COPY(StatusChangerManager)

	static StatusChangerManager *Instance;

	QList<StatusChanger *> statusChangers;
	Status LastStatus;
	bool enabled;

	StatusChangerManager();
	virtual ~StatusChangerManager();

public:
	static StatusChangerManager * instance();

	void enable();

	void registerStatusChanger(StatusChanger *statusChanger);
	void unregisterStatusChanger(StatusChanger *statusChanger);

	Status status() { return LastStatus; }

public slots:
	void statusChanged();

signals:
	void statusChanged(Status);

};

#endif // STATUS_CHANGER_MANAGER_H
