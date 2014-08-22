/*
 * %kadu copyright begin%
 * Copyright 2008, 2009, 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2008 Michał Podsiadlik (michal@kadu.net)
 * Copyright 2007, 2008, 2009, 2010, 2011, 2013 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2011 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2007, 2008 Dawid Stawiarski (neeo@kadu.net)
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

#include "buddies/buddy-list.h"
#include "buddies/group.h"
#include "gui/widgets/group-tab-bar/group-tab-bar-configuration.h"

class Chat;
class GroupFilter;

class KADUAPI GroupTabBar : public QTabBar
{
	Q_OBJECT

	GroupTabBarConfiguration Configuration;

	// for dnd support
	BuddyList DNDBuddies;
	QList<Chat> DNDChats;

	int indexOf(GroupFilter groupFilter);
	void insertGroupFilter(int index, GroupFilter groupFilter);
	void removeGroupFilter(GroupFilter groupFilter);
	void updateTabData(int tabIndex, GroupFilter groupFilter);

	void updateUngrouppedTab();
	bool shouldShowUngrouppedTab() const;

private slots:
	void addGroup(Group group);
	void removeGroup(Group group);
	void updateGroup(Group group);

	void currentChangedSlot(int index);

	void addBuddy();
	void renameGroup();
	void deleteGroup();
	void createNewGroup();
	void groupProperties();

 	void addToGroup();
 	void moveToGroup();

protected:
	virtual void contextMenuEvent(QContextMenuEvent *event);

	virtual void dragEnterEvent(QDragEnterEvent *event);
	virtual void dragMoveEvent(QDragMoveEvent *event);
	virtual void dropEvent(QDropEvent *event);

public:
	explicit GroupTabBar(QWidget *parent = 0);
	virtual ~GroupTabBar();

	void setInitialConfiguration(GroupTabBarConfiguration configuration);
	void setConfiguration(GroupTabBarConfiguration configuration);
	GroupTabBarConfiguration configuration();

	Group groupAt(int index) const;
	GroupFilter groupFilter() const;
	GroupFilter groupFilterAt(int index) const;
	QVector<GroupFilter> groupFilters() const;

signals:
	void currentGroupFilterChanged(const GroupFilter &groupFilter);

};

#endif // GROUP_TAB_BAR_H
