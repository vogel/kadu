/*
 * %kadu copyright begin%
 * Copyright 2011 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2009, 2010 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009, 2010, 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#ifndef GROUP_TAB_BAR_H
#define GROUP_TAB_BAR_H

#include <QtGui/QTabBar>

#include "configuration/configuration-aware-object.h"
#include "buddies/buddy-list.h"
#include "buddies/group.h"

class GroupBuddyFilter;

class KADUAPI GroupTabBar : public QTabBar, ConfigurationAwareObject
{
	Q_OBJECT

	GroupBuddyFilter *Filter;
	// for dnd support
	Group currentGroup;
	BuddyList currentBuddies;

	bool ShowAllGroup;

	int AutoGroupTabPosition;

	void updateGroup(Group group);
	bool hasAnyUngrouppedBuddy() const;

	void updateAutoGroupTab(bool oldShowAllGroup);

private slots:
	void updateAutoGroupTab();
	void currentChangedSlot(int index);
	void groupAdded(Group group);
	void groupRemoved(Group group);

	void groupUpdated();

	void addBuddy();
	void renameGroup();
	void deleteGroup();
	void createNewGroup();
	void groupProperties();

 	void addToGroup();
 	void moveToGroup();

	void saveGroupTabsPosition();

protected:
	virtual void contextMenuEvent(QContextMenuEvent *event);

	virtual void dragEnterEvent(QDragEnterEvent *event);
	virtual void dropEvent(QDropEvent *event);

	virtual void configurationUpdated();

public:
	explicit GroupTabBar(QWidget *parent = 0);
	virtual ~GroupTabBar();

	void addGroup(const Group &group);

	GroupBuddyFilter * filter() { return Filter; }

signals:
	void currentGroupChanged(const Group &group);

};

#endif // GROUP_TAB_BAR_H
