/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef PERSONAL_INFO_SERVICE_H
#define PERSONAL_INFO_SERVICE_H

#include <QtCore/QObject>

#include "buddies/buddy.h"

class PersonalInfoService : public QObject
{
	Q_OBJECT

public:
	PersonalInfoService(QObject *parent = 0) : QObject(parent) {}

	virtual void updatePersonalInfo(Contact contact) = 0;
	virtual void fetchPersonalInfo() = 0;

signals:
	void personalInfoAvailable(Contact contact);
	void personalInfoUpdated(bool);

};

#endif // PERSONAL_INFO_SERVICE_H
