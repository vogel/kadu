/*
 * %kadu copyright begin%
 * Copyright 2009, 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009, 2010, 2011, 2013 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2010, 2011, 2013 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * %kadu copyright end%
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef GROUP_MANAGER_H
#define GROUP_MANAGER_H

#include <QtCore/QList>
#include <QtCore/QObject>
#include <QtCore/QString>

#include "buddies/group.h"
#include "storage/simple-manager.h"

#include "exports.h"

class KADUAPI GroupManager : public QObject, public SimpleManager<Group>
{
	Q_OBJECT
	Q_DISABLE_COPY(GroupManager)

	static GroupManager *Instance;

	GroupManager();
	virtual ~GroupManager();

	void init();

	void importConfiguration();

private slots:
	void groupDataUpdated();

protected:
	virtual void load();
	virtual void store();

	virtual void itemAboutToBeAdded(Group item);
	virtual void itemAdded(Group item);
	virtual void itemAboutToBeRemoved(Group item);
	virtual void itemRemoved(Group item);

public:
	static GroupManager * instance();

	virtual QString storageNodeName() { return QLatin1String("Groups"); }
	virtual QString storageNodeItemName() { return QLatin1String("Group"); }

	Group byName(const QString &name, bool create = true);

	bool acceptableGroupName(const QString &groupName, bool acceptExistingGroupName = false);

signals:
	void groupAboutToBeAdded(Group group);
	void groupAdded(Group group);
	void groupAboutToBeRemoved(Group group);
	void groupRemoved(Group group);

	void groupUpdated(Group group);

	void saveGroupData();

};

#endif // GROUP_MANAGER_H
