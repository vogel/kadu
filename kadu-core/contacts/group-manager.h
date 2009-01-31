/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef GROUP_MANAGER_H
#define GROUP_MANAGER_H

#include <QtCore/QList>
#include <QtCore/QObject>
#include <QtCore/QString>

#include "configuration/storable-object.h"

#include "exports.h"

class Group;

class KADUAPI GroupManager : public QObject, public StorableObject
{
	Q_OBJECT
	Q_DISABLE_COPY(GroupManager)

	static GroupManager *Instance;

	GroupManager();
	~GroupManager();

	QList<Group *> Groups;

	void importConfiguration();

protected:
	virtual StoragePoint * createStoragePoint() const;

public:
	static GroupManager * instance();

	void loadConfiguration();
	void storeConfiguration();

	QList<Group *> groups() { return Groups; }

	void addGroup(Group *newGroup);

	Group * byUuid(const QString &uuid) const;
	Group * byName(const QString &name, bool create = true);

signals:
	void groupAboutToBeAdded(Group *group);
	void groupAdded(Group *group);
	void groupAboutToBeRemoved(Group *group);
	void groupRemoved(Group *group);

};

#endif // GROUP_MANAGER_H
