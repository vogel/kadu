/*
 * %kadu copyright begin%
 * Copyright 2010 Tomasz Rostanski (rozteck@interia.pl)
 * Copyright 2008, 2009, 2010, 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009, 2010 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2004 Tomasz Jarzynka (tomee@cpi.pl)
 * Copyright 2008, 2009 Michał Podsiadlik (michal@kadu.net)
 * Copyright 2009 Bartłomiej Zimoń (uzi18@o2.pl)
 * Copyright 2002, 2003, 2005 Adrian Smarzewski (adrian@kadu.net)
 * Copyright 2005 Paweł Płuciennik (pawel_p@kadu.net)
 * Copyright 2002, 2003 Tomasz Chiliński (chilek@chilan.com)
 * Copyright 2007, 2008, 2009, 2010, 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2010, 2011 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2007 Dawid Stawiarski (neeo@kadu.net)
 * Copyright 2004, 2005, 2006, 2007 Marcin Ślusarz (joi@kadu.net)
 * Copyright 2002, 2003 Dariusz Jagodzik (mast3r@kadu.net)
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

#include <QtGui/QApplication>
#include <QtGui/QDragEnterEvent>
#include <QtGui/QInputDialog>
#include <QtGui/QMenu>

#include "buddies/buddy-list-mime-data-helper.h"
#include "buddies/buddy-manager.h"
#include "buddies/group-manager.h"
#include "buddies/group.h"
#include "chat/chat-list-mime-data-helper.h"
#include "configuration/configuration-file.h"
#include "core/core.h"
#include "gui/windows/add-buddy-window.h"
#include "gui/windows/group-properties-window.h"
#include "gui/windows/kadu-window.h"
#include "gui/windows/message-dialog.h"
#include "icons/kadu-icon.h"

#include "debug.h"

#include "group-tab-bar.h"

static bool compareGroups(Group g1, Group g2)
{
	return g1.tabPosition() < g2.tabPosition();
}

GroupTabBar::GroupTabBar(QWidget *parent) :
		QTabBar(parent), ShowAllGroup(true), HadAnyUngrouppedBuddy(false), AutoGroupTabPosition(-1)
{
	setSizePolicy(QSizePolicy(QSizePolicy::Minimum, QSizePolicy::Expanding));

#ifdef Q_OS_MAC
	setDocumentMode(true);
	setUsesScrollButtons(true);
#endif

 	setAcceptDrops(true);
	setDrawBase(false);
	setMovable(true);

	setShape(QTabBar::RoundedWest);
	setIconSize(QSize(16, 16));

	QList<Group> groups = GroupManager::instance()->items().toList();
	qStableSort(groups.begin(), groups.end(), compareGroups);
	foreach (const Group &group, groups)
		addGroup(group);

	connect(this, SIGNAL(currentChanged(int)), this, SLOT(currentChangedSlot(int)));

	connect(GroupManager::instance(), SIGNAL(groupAdded(Group)), this, SLOT(groupAdded(Group)));
	connect(GroupManager::instance(), SIGNAL(groupAboutToBeRemoved(Group)), this, SLOT(groupRemoved(Group)));
	connect(GroupManager::instance(), SIGNAL(saveGroupData()), this, SLOT(saveGroupTabsPosition()));

	ShowAllGroup = config_file.readBoolEntry("Look", "ShowGroupAll", true);
	// !ShowAllGroup to force update
	updateAutoGroupTab(!ShowAllGroup);

	if (!config_file.readBoolEntry("Look", "DisplayGroupTabs", true))
	{
		setCurrentIndex(AutoGroupTabPosition);
		setVisible(false);
		return;
	}

	int currentGroup = config_file.readNumEntry("Look", "CurrentGroupTab", 0);
	if (currentGroup == currentIndex())
		currentChangedSlot(currentGroup);
	else
		setCurrentIndex(currentGroup);

	if (!ShowAllGroup)
		connect(BuddyManager::instance(), SIGNAL(buddyUpdated(Buddy)), this, SLOT(checkForUngroupedBuddies()));
}

GroupTabBar::~GroupTabBar()
{
	if (ShowAllGroup)
		config_file.writeEntry("Look", "AllGroupTabPosition", AutoGroupTabPosition);
	else
		config_file.writeEntry("Look", "UngroupedGroupTabPosition", AutoGroupTabPosition);

	config_file.writeEntry("Look", "CurrentGroupTab", currentIndex());
}

void GroupTabBar::updateAutoGroupTab(bool oldShowAllGroup)
{
	// update only if we care about hasAnyUngrouppedBuddy() or ShowAllGroup has changed
	if (ShowAllGroup == true && oldShowAllGroup == true)
		return;

	if (AutoGroupTabPosition != -1)
	{
		if (oldShowAllGroup)
			config_file.writeEntry("Look", "AllGroupTabPosition", AutoGroupTabPosition);
		else
			config_file.writeEntry("Look", "UngroupedGroupTabPosition", AutoGroupTabPosition);
	}

	bool haveAnyUngroupedBuddy = hasAnyUngrouppedBuddy();
	bool currentWasAutoGroup = false;
	if (-1 != AutoGroupTabPosition && tabData(AutoGroupTabPosition) == "AutoTab")
	{
		if (AutoGroupTabPosition == currentIndex())
			currentWasAutoGroup = true;
		if (!ShowAllGroup && !haveAnyUngroupedBuddy)
			removeTab(AutoGroupTabPosition);
	}
	else
		AutoGroupTabPosition = -1;

	if (ShowAllGroup)
	{
		int oldAutoGroupTabPosition = AutoGroupTabPosition;
		AutoGroupTabPosition = config_file.readNumEntry("Look", "AllGroupTabPosition", 0);

		if (oldAutoGroupTabPosition == -1)
			insertTab(AutoGroupTabPosition, KaduIcon("x-office-address-book").icon(), tr("All"));
		else
		{
			moveTab(oldAutoGroupTabPosition, AutoGroupTabPosition);
			setTabIcon(AutoGroupTabPosition, KaduIcon("x-office-address-book").icon());
			setTabText(AutoGroupTabPosition, tr("All"));
		}
	}
	else if (haveAnyUngroupedBuddy)
	{
		int oldAutoGroupTabPosition = AutoGroupTabPosition;
		AutoGroupTabPosition = config_file.readNumEntry("Look", "UngroupedGroupTabPosition", -1);

		if (oldAutoGroupTabPosition == -1)
			insertTab(AutoGroupTabPosition, tr("Ungrouped"));
		else
		{
			moveTab(oldAutoGroupTabPosition, AutoGroupTabPosition);
			setTabIcon(AutoGroupTabPosition, QIcon());
			setTabText(AutoGroupTabPosition, tr("Ungrouped"));
		}
	}
	else
		AutoGroupTabPosition = -1;

	if (-1 != AutoGroupTabPosition)
	{
		setTabData(AutoGroupTabPosition, "AutoTab");
		if (currentWasAutoGroup)
		{
			if (AutoGroupTabPosition == currentIndex())
				currentChangedSlot(AutoGroupTabPosition);
			else
				setCurrentIndex(AutoGroupTabPosition);
		}
	}
}

void GroupTabBar::addGroup(const Group &group)
{
	int index = addTab(group.name());
	setTabData(index, group.uuid().toString());
	connect(group, SIGNAL(updated()), this, SLOT(groupUpdated()));

	updateGroup(group);
}

Group GroupTabBar::group() const
{
	return GroupManager::instance()->byUuid(tabData(currentIndex()).toString());
}

void GroupTabBar::currentChangedSlot(int index)
{
	emit currentGroupChanged(GroupManager::instance()->byUuid(tabData(index).toString()));
}

void GroupTabBar::groupAdded(Group group)
{
	QString groupUuid = group.uuid().toString();
	for (int i = 0; i < count(); ++i)
		if (tabData(i).toString() == groupUuid) //group is already in tabbar
			return;
	addGroup(group);
}

void GroupTabBar::groupRemoved(Group group)
{
	QString groupUuid = group.uuid().toString();
	for (int i = 0; i < count(); ++i)
		if (tabData(i).toString() == groupUuid)
		{
			removeTab(i);
			return;
		}
}

void GroupTabBar::updateGroup(Group group)
{
	QString groupUuid = group.uuid().toString();
	int groupId = -1;
	for (int i = 0; i < count(); ++i)
		if (tabData(i).toString() == groupUuid)
			groupId = i;

	if (-1 == groupId)
		return;

	setTabIcon(groupId, QIcon(group.showIcon() ? group.icon() : QString()));
	setTabText(groupId, group.showName() ? group.name() : QString());

	if (group.showName())
		setTabText(groupId, group.name());
	else
		setTabText(groupId, QString());
}

bool GroupTabBar::hasAnyUngrouppedBuddy()
{
	foreach (const Buddy &buddy, BuddyManager::instance()->items())
		if (!buddy.isAnonymous() && buddy.groups().isEmpty())
		{
			HadAnyUngrouppedBuddy = true;
			return true;
		}

	HadAnyUngrouppedBuddy = false;
	return false;
}

void GroupTabBar::checkForUngroupedBuddies()
{
	// hasAnyUngrouppedBuddy() overwrites HadAnyUngrouppedBuddy
	bool oldHadAnyUngrouppedBuddy = HadAnyUngrouppedBuddy;
	if (oldHadAnyUngrouppedBuddy != hasAnyUngrouppedBuddy())
	{
		saveGroupTabsPosition();
		updateAutoGroupTab(ShowAllGroup);
	}
}

void GroupTabBar::groupUpdated()
{
	Group group = sender();
	if (group.isNull())
		return;

	updateGroup(group);
}

void GroupTabBar::contextMenuEvent(QContextMenuEvent *event)
{
	int tabIndex = tabAt(event->pos());

	const Group &group = tabIndex == -1
	        ? Group::null
	        : GroupManager::instance()->byUuid(tabData(tabIndex).toString());

	QMenu menu;

	QAction *addBuddyAction = menu.addAction(tr("Add Buddy"), this, SLOT(addBuddy()));
	addBuddyAction->setEnabled(group);
	addBuddyAction->setData(group);

	QAction *renameGroupAction = menu.addAction(tr("Rename Group"), this, SLOT(renameGroup()));
	renameGroupAction->setEnabled(group);
	renameGroupAction->setData(group);

	menu.addSeparator();

	QAction *deleteGroupAction = menu.addAction(tr("Delete Group"), this, SLOT(deleteGroup()));
	deleteGroupAction->setEnabled(group);
	deleteGroupAction->setData(group);

	menu.addAction(tr("Add Group"), this, SLOT(createNewGroup()));

	menu.addSeparator();

	QAction *propertiesAction = menu.addAction(tr("Properties"), this, SLOT(groupProperties()));
	propertiesAction->setEnabled(group);
	propertiesAction->setData(group);

	menu.exec(event->globalPos());
}

void GroupTabBar::dragEnterEvent(QDragEnterEvent *event)
{
	QTabBar::dragEnterEvent(event);

	if (event->mimeData()->hasFormat("application/x-kadu-buddy-list"))
	{
		event->setDropAction(Qt::LinkAction);
		event->accept();
	}

	if (event->mimeData()->hasFormat("application/x-kadu-chat-list"))
	{
		event->setDropAction(Qt::LinkAction);
		event->accept();
	}
}

void GroupTabBar::dragMoveEvent(QDragMoveEvent *event)
{
	QTabBar::dragMoveEvent(event);

	if (event->mimeData()->hasFormat("application/x-kadu-buddy-list"))
	{
		event->setDropAction(Qt::LinkAction);
		event->accept();
	}

	if (event->mimeData()->hasFormat("application/x-kadu-chat-list"))
	{
		event->setDropAction(Qt::LinkAction);
		event->accept();
	}
}

void GroupTabBar::dropEvent(QDropEvent *event)
{
	QTabBar::dropEvent(event);

	if (!event->mimeData()->hasFormat("application/x-kadu-buddy-list") &&
	        !event->mimeData()->hasFormat("application/x-kadu-chat-list"))
	{
		event->ignore();
		return;
	}

	event->acceptProposedAction();

	BuddyList buddies = BuddyListMimeDataHelper::fromMimeData(event->mimeData());
	QList<Chat> chats = ChatListMimeDataHelper::fromMimeData(event->mimeData());

	QApplication::setOverrideCursor(Qt::ArrowCursor);
	int tabIndex = tabAt(event->pos());

	if (tabIndex == -1)
	{
		bool ok;
		QString newGroupName;
		do
		{
			newGroupName = QInputDialog::getText(this, tr("New Group"),
				tr("Please enter the name for the new group:"), QLineEdit::Normal,
				QString(), &ok);

			if (!ok)
			{
				QApplication::restoreOverrideCursor();
				return;
			}

			ok = GroupManager::instance()->acceptableGroupName(newGroupName);
		}
		while (!ok);

		Group group = GroupManager::instance()->byName(newGroupName);

		foreach (const Buddy &buddy, buddies)
			buddy.addToGroup(group);
		foreach (const Chat &chat, chats)
			chat.addToGroup(group);

		QApplication::restoreOverrideCursor();

		return;
	}

	Group clickedGroup = GroupManager::instance()->byUuid(tabData(tabIndex).toString());

	DNDBuddies = buddies;
	DNDChats = chats;

	if (clickedGroup)
	{
		QMenu menu;
		menu.addAction(tr("Move to group %1").arg(clickedGroup.name()), this, SLOT(moveToGroup()))->setData(clickedGroup);
		menu.addAction(tr("Add to group %1").arg(clickedGroup.name()), this, SLOT(addToGroup()))->setData(clickedGroup);
		menu.exec(QCursor::pos());
	}

	QApplication::restoreOverrideCursor();

	kdebugf2();
}

void GroupTabBar::addBuddy()
{
	QAction *action = qobject_cast<QAction *>(sender());
	if (!action)
		return;

	AddBuddyWindow *addBuddyWindow = new AddBuddyWindow(Core::instance()->kaduWindow());
	addBuddyWindow->setGroup(action->data().value<Group>());
	addBuddyWindow->show();
}

void GroupTabBar::renameGroup()
{
	QAction *action = qobject_cast<QAction *>(sender());
	if (!action)
		return;

	const Group &group = action->data().value<Group>();
	if (!group)
		return;

	bool ok;
	QString newGroupName = QInputDialog::getText(this, tr("Rename Group"),
				tr("Please enter a new name for this group"), QLineEdit::Normal,
				group.name(), &ok);

	if (ok && newGroupName != group.name() && GroupManager::instance()->acceptableGroupName(newGroupName))
		group.setName(newGroupName);
}

void GroupTabBar::deleteGroup()
{
	QAction *action = qobject_cast<QAction *>(sender());
	if (!action)
		return;

	const Group &group = action->data().value<Group>();
	if (!group)
		return;

	if (group && MessageDialog::ask(KaduIcon("dialog-warning"), tr("Kadu"), tr("Selected group:\n%0 will be deleted. Are you sure?").arg(group.name()), Core::instance()->kaduWindow()))
		GroupManager::instance()->removeItem(group);
}

void GroupTabBar::createNewGroup()
{
	bool ok;
	QString newGroupName = QInputDialog::getText(this, tr("New Group"),
				tr("Please enter the name for the new group:"), QLineEdit::Normal,
				QString(), &ok);

	if (ok && GroupManager::instance()->acceptableGroupName(newGroupName))
		GroupManager::instance()->byName(newGroupName);
}

void GroupTabBar::groupProperties()
{
	QAction *action = qobject_cast<QAction *>(sender());
	if (!action)
		return;

	const Group &group = action->data().value<Group>();
	if (group)
		(new GroupPropertiesWindow(group, Core::instance()->kaduWindow()))->show();
}

void GroupTabBar::addToGroup()
{
	QAction *action = qobject_cast<QAction *>(sender());
	if (!action)
		return;

	const Group &group = action->data().value<Group>();

	foreach (const Buddy &buddy, DNDBuddies)
		buddy.addToGroup(group);
	foreach (const Chat &chat, DNDChats)
		chat.addToGroup(group);
}

void GroupTabBar::moveToGroup()
{
	QAction *action = qobject_cast<QAction *>(sender());
	if (!action)
		return;

	const Group &removeFromGroup = GroupManager::instance()->byUuid(tabData(currentIndex()).toString());
	const Group &group = action->data().value<Group>();

	foreach (const Buddy &buddy, DNDBuddies)
	{
		buddy.removeFromGroup(removeFromGroup);
		buddy.addToGroup(group);
	}

	foreach (const Chat &chat, DNDChats)
	{
		chat.removeFromGroup(removeFromGroup);
		chat.addToGroup(group);
	}
}

void GroupTabBar::saveGroupTabsPosition()
{
	AutoGroupTabPosition = -1;
	for (int i = 0; i < count(); ++i)
	{
		Group group(GroupManager::instance()->byUuid(tabData(i).toString()));
		if (group)
			group.setTabPosition(i);
		else
			AutoGroupTabPosition = i;
	}
}

void GroupTabBar::configurationUpdated()
{
	if (!config_file.readBoolEntry("Look", "DisplayGroupTabs", true))
	{
		disconnect(BuddyManager::instance(), SIGNAL(buddyUpdated(Buddy)), this, SLOT(checkForUngroupedBuddies()));

		for (int i = 0; i < count(); ++i)
			if (!GroupManager::instance()->byUuid(tabData(i).toString()))
			{
				setCurrentIndex(i);
				break;
			}

		setVisible(false);
		return;
	}

	setVisible(true);

	saveGroupTabsPosition();
	bool oldShowAllGroup = ShowAllGroup;
	ShowAllGroup = config_file.readBoolEntry("Look", "ShowGroupAll", true);
	if (oldShowAllGroup != ShowAllGroup)
	{
		updateAutoGroupTab(oldShowAllGroup);

		if (ShowAllGroup)
			disconnect(BuddyManager::instance(), SIGNAL(buddyUpdated(Buddy)), this, SLOT(checkForUngroupedBuddies()));
		else
			connect(BuddyManager::instance(), SIGNAL(buddyUpdated(Buddy)), this, SLOT(checkForUngroupedBuddies()));
	}
}
