/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef GROUP_H
#define GROUP_H

#include <QtCore/QUuid>

#include "configuration/storable-object.h"

class Group : private StorableObject
{
	QUuid Uuid;
	QString Name;
	QString Icon;

protected:
	virtual StoragePoint * createStoragePoint() const;

public:
	static Group * loadFromStorage(StoragePoint *groupStoragePoint);

	Group(QUuid uuid = QUuid());
	~Group();

	void importConfiguration(const QString &name);
	void loadConfiguration();
	void storeConfiguration();

	QUuid uuid() const { return Uuid; }
	QString name() const { return Name; }
	QString icon() const { return Icon; }

	void setName(const QString &name) { Name = name; }

};

#endif // GROUP_H
