/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef CONFERENCE_H
#define CONFERENCE_H

#include <QtCore/QObject>
#include <QtCore/QUuid>

#include "accounts/account.h"
#include "configuration/storable-object.h"

class XmlConfigFile;

class Conference : public StorableObject
{
	Account *CurrentAccount;
	QUuid Uuid;

protected:
	virtual StoragePoint * createStoragePoint() const;

public:
	Conference(Account *parentAccount, QUuid uuid = QUuid());
	virtual ~Conference();

	QUuid uuid() const;
	Account *account() { return CurrentAccount; }
	static Conference * loadFromStorage(StoragePoint *conferenceStoragePoint);

	virtual void loadConfiguration();
	virtual void storeConfiguration();

};

#endif // CONFERENCE_H
