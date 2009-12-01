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

#include "storage/simple-manager.h"

#include "exports.h"

class Group;

class KADUAPI GroupManager : public QObject, public SimpleManager<Group>
{
	Q_OBJECT
	Q_DISABLE_COPY(GroupManager)

	static GroupManager *Instance;

	GroupManager();
	virtual ~GroupManager();

	void importConfiguration();

protected:
	virtual void load();

	virtual void itemAboutToBeAdded(Group item);
	virtual void itemAdded(Group item);
	virtual void itemAboutToBeRemoved(Group item);
	virtual void itemRemoved(Group item);

public:
	static GroupManager * instance();

	virtual QString storageNodeName() { return QLatin1String("Groups"); }
	virtual QString storageNodeItemName() { return QLatin1String("Group"); }

	Group byName(const QString &name, bool create = true);

	bool acceptableGroupName(const QString &groupName);

signals:
	void groupAboutToBeAdded(Group group);
	void groupAdded(Group group);
	void groupAboutToBeRemoved(Group group);
	void groupRemoved(Group group);

	void saveGroupData();

};

#include "buddies/group.h" // for MOC

#endif // GROUP_MANAGER_H
