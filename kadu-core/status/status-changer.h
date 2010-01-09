/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef STATUS_CHANGER_H
#define STATUS_CHANGER_H

#include <QtCore/QObject>

#include "exports.h"

class Status;
class StatusContainer;

class KADUAPI StatusChanger : public QObject
{
	Q_OBJECT

	int Priority;

public:
	StatusChanger(int priority);
	virtual ~StatusChanger();

	int priority();

	virtual void changeStatus(StatusContainer *container, Status &status) = 0;

signals:
	void statusChanged(StatusContainer *);

};

#endif // STATUS_CHANGER_H
