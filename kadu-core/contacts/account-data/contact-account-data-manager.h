/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef CONTACT_ACCOUNT_DATA_MANAGER_H
#define CONTACT_ACCOUNT_DATA_MANAGER_H

#include <QtCore/QMap>
#include <QtCore/QObject>
#include <QtCore/QUuid>

#include "configuration/storable-object.h"

#include "exports.h"

class ContactAccountData;

class KADUAPI ContactAccountDataManager : public QObject, public StorableObject
{
	Q_OBJECT
	Q_DISABLE_COPY(ContactAccountDataManager)

	static ContactAccountDataManager * Instance;

	QList<ContactAccountData *> ContactAccountDataList;

	ContactAccountDataManager();
	virtual ~ContactAccountDataManager();

protected:
	virtual StoragePoint * createStoragePoint();

public:
	static ContactAccountDataManager * instance();

	virtual void load();
	virtual void store();

	void addContactAccountData(ContactAccountData *cad);
	void removeContactAccountData(ContactAccountData *cad);
	
	unsigned int count() { return ContactAccountDataList.count(); }

	ContactAccountData * byIndex(unsigned int index);
	ContactAccountData * byUuid(const QString &uuid);

signals:
	void contactAccountDataAboutToBeAdded(ContactAccountData *cad);
	void contactAccountDataAdded(ContactAccountData *cad);
	void contactAccountDataAboutToBeRemoved(ContactAccountData *cad);
	void contactAccountDataRemoved(ContactAccountData *cad);

};

#endif // CONTACT_ACCOUNT_DATA_MANAGER_H
