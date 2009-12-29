/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef USER_STATUS_CHANGER_H
#define USER_STATUS_CHANGER_H

#include <QtCore/QMap>

#include "status/status.h"

#include "status/status-changer.h"

class KADUAPI UserStatusChanger : public StatusChanger
{
	Q_OBJECT

	QMap<StatusContainer *, Status> UserStatuses;

public:
	UserStatusChanger();
	virtual ~UserStatusChanger();

	virtual void changeStatus(StatusContainer *, Status &status);
	Status status(StatusContainer *);

public slots:
	void userStatusSet(StatusContainer *container, const Status &status);

};

#endif // USER_STATUS_CHANGER_H
