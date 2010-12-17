/*
 * %kadu copyright begin%
 * Copyright 2007 Dawid Stawiarski (neeo@kadu.net)
 * Copyright 2004 Tomasz Jarzynka (tomee@cpi.pl)
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2004, 2005, 2006, 2007 Marcin Ślusarz (joi@kadu.net)
 * Copyright 2003, 2005 Adrian Smarzewski (adrian@kadu.net)
 * Copyright 2003 Tomasz Chiliński (chilek@chilan.com)
 * Copyright 2007, 2008, 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2008, 2009 Michał Podsiadlik (michal@kadu.net)
 * Copyright 2010 Tomasz Rostanski (rozteck@interia.pl)
 * Copyright 2009 Bartłomiej Zimoń (uzi18@o2.pl)
 * Copyright 2008, 2009, 2010 Piotr Galiszewski (piotrgaliszewski@gmail.com)
 * Copyright 2005 Paweł Płuciennik (pawel_p@kadu.net)
 * Copyright 2003 Dariusz Jagodzik (mast3r@kadu.net)
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

#include "configuration/configuration-file.h"
#include "buddies/buddy-list-mime-data-helper.h"
#include "buddies/group.h"
#include "buddies/group-manager.h"
#include "buddies/filter/group-buddy-filter.h"
#include "core/core.h"
#include "gui/windows/add-buddy-window.h"
#include "gui/windows/group-properties-window.h"
#include "gui/windows/kadu-window.h"
#include "gui/windows/message-dialog.h"

#include "debug.h"
#include "icons-manager.h"

#include "group-tab-bar.h"

 bool compareGroups(Group g1, Group g2)
 {
     return g1.tabPosition() < g2.tabPosition();
 }

GroupTabBar::GroupTabBar(QWidget *parent)
	: QTabBar(parent), showAllGroup(true)
{
	Filter = new GroupBuddyFilter(this);

	setSizePolicy(QSizePolicy(QSizePolicy::Minimum, QSizePolicy::Expanding));

#ifdef Q_OS_MAC
	setDocumentMode(true);
	setUsesScrollButtons(true);
#endif

 	setAcceptDrops(true);
	setDrawBase(false);
	setMovable(true);

	setShape(QTabBar::RoundedWest);

	setFont(QFont(config_file.readFontEntry("Look", "UserboxFont").family(),
			config_file.readFontEntry("Look", "UserboxFont").pointSize(), QFont::Bold));
	setIconSize(QSize(16, 16));

	QList<Group> groups = GroupManager::instance()->items();
	qStableSort(groups.begin(), groups.end(), compareGroups);
	foreach (const Group group, groups)
		addGroup(group);

	connect(this, SIGNAL(currentChanged(int)), this, SLOT(currentChangedSlot(int)));

	connect(GroupManager::instance(), SIGNAL(groupAdded(Group)), this, SLOT(groupAdded(Group)));
	connect(GroupManager::instance(), SIGNAL(groupAboutToBeRemoved(Group)), this, SLOT(groupRemoved(Group)));
	connect(GroupManager::instance(), SIGNAL(saveGroupData()), this, SLOT(saveGroupTabsPosition()));

	showAllGroup = config_file.readBoolEntry("Look", "ShowGroupAll", true);
	if (showAllGroup)
	{
		AutoGroupTabPosition = config_file.readNumEntry("Look", "AllGroupTabPosition", 0);
		insertTab(AutoGroupTabPosition, IconsManager::instance()->iconByPath("x-office-address-book"), tr("All"));
	}
	else
	{
		AutoGroupTabPosition = config_file.readNumEntry("Look", "UngroupedGroupTabPosition", 100);
		insertTab(AutoGroupTabPosition, tr("Ungrouped"));
	}
	setTabData(AutoGroupTabPosition, "AutoTab");

	if (!config_file.readBoolEntry("Look", "DisplayGroupTabs", true))
	{
		Filter->setAllGroupShown(true);
		setCurrentIndex(AutoGroupTabPosition);
		setVisible(false);
		return;
	}

	Filter->setAllGroupShown(showAllGroup);

	int currentGroup = config_file.readNumEntry("Look", "CurrentGroupTab", 0);
	setCurrentIndex(currentGroup);
	currentChangedSlot(currentGroup);
}

GroupTabBar::~GroupTabBar()
{
	if (showAllGroup)
		config_file.writeEntry("Look", "AllGroupTabPosition", AutoGroupTabPosition);
	else
		config_file.writeEntry("Look", "UngroupedGroupTabPosition", AutoGroupTabPosition);

	config_file.writeEntry("Look", "CurrentGroupTab", currentIndex());
}

void GroupTabBar::addGroup(const Group group)
{
	int index = addTab(group.name());
	setTabData(index, group.uuid().toString());
	connect(group, SIGNAL(updated()), this, SLOT(groupUpdated()));

	updateGroup(group);
}

void GroupTabBar::currentChangedSlot(int index)
{
	Group group = GroupManager::instance()->byUuid(tabData(index).toString());
	emit currentGroupChanged(group);
	Filter->setGroup(group);
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
	if (tabData(currentIndex()).toString() == "AutoTab")
		Filter->refresh();

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

	if (tabIndex != -1)
		currentGroup= GroupManager::instance()->byUuid(tabData(tabIndex).toString());

	QMenu *menu = new QMenu(this);

	menu->addAction(tr("Add Buddy"), this, SLOT(addBuddy()))->setEnabled(tabIndex != -1 && currentGroup);
	menu->addAction(tr("Rename Group"), this, SLOT(renameGroup()))->setEnabled(tabIndex != -1 && currentGroup);
	menu->addSeparator();
	menu->addAction(tr("Delete Group"), this, SLOT(deleteGroup()))->setEnabled(tabIndex != -1 && currentGroup);
	menu->addAction(tr("Create New Group"), this, SLOT(createNewGroup()));
	menu->addSeparator();
	menu->addAction(tr("Properties"), this, SLOT(groupProperties()))->setEnabled(tabIndex != -1 && currentGroup);

	menu->popup(event->globalPos());
}

void GroupTabBar::dragEnterEvent(QDragEnterEvent *event)
{
	if (event->mimeData()->hasFormat("application/x-kadu-ules"))
		event->acceptProposedAction();
}

void GroupTabBar::dropEvent(QDropEvent *event)
{
	kdebugf();

	QStringList ules;
	if (!event->mimeData()->hasFormat("application/x-kadu-ules"))
		return;

	event->acceptProposedAction();

	BuddyList buddies = BuddyListMimeDataHelper::fromMimeData(event->mimeData());

	QApplication::setOverrideCursor(QCursor(Qt::ArrowCursor));
	QString groupUuid, groupName;
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
				return;

			ok = !newGroupName.isEmpty() && GroupManager::instance()->acceptableGroupName(newGroupName);
		}
		while (!ok);

		Group group = GroupManager::instance()->byName(newGroupName);

		foreach (Buddy buddy, buddies)
			buddy.addToGroup(group);

		QApplication::restoreOverrideCursor();

		return;
	}
	else
		currentGroup= GroupManager::instance()->byUuid(tabData(tabIndex).toString());

	currentBuddies = buddies;

	QMenu menu(this);
	if (currentGroup)
	{
		menu.addAction(tr("Move to group %1").arg(currentGroup.name()), this, SLOT(moveToGroup()))
				->setEnabled(tabData(currentIndex()).toString() != "AutoTab");
		menu.addAction(tr("Add to group %1").arg(currentGroup.name()), this, SLOT(addToGroup()));
	}

	menu.exec(QCursor::pos());

	QApplication::restoreOverrideCursor();

	kdebugf2();
}

void GroupTabBar::addBuddy()
{
	if (!currentGroup)
		return;

	AddBuddyWindow *addBuddyWindow = new AddBuddyWindow(Core::instance()->kaduWindow());
	addBuddyWindow->setGroup(currentGroup);
	addBuddyWindow->show();
}


void GroupTabBar::renameGroup()
{
	if (!currentGroup)
		return;
	bool ok;
	QString newGroupName = QInputDialog::getText(this, tr("Rename Group"),
				tr("Please enter a new name for this group"), QLineEdit::Normal,
				currentGroup.name(), &ok);

	if (ok && !newGroupName.isEmpty() && newGroupName != currentGroup.name() && GroupManager::instance()->acceptableGroupName(newGroupName))
		currentGroup.setName(newGroupName);
}

void GroupTabBar::deleteGroup()
{
	if (currentGroup && MessageDialog::ask("dialog-warning", tr("Kadu"), tr("Selected group:\n%0 will be deleted. Are you sure?").arg(currentGroup.name()), Core::instance()->kaduWindow()))
		GroupManager::instance()->removeItem(currentGroup);
}

void GroupTabBar::createNewGroup()
{
	bool ok;
	QString newGroupName = QInputDialog::getText(this, tr("New Group"),
				tr("Please enter the name for the new group:"), QLineEdit::Normal,
				QString(), &ok);

	if (ok && !newGroupName.isEmpty() && GroupManager::instance()->acceptableGroupName(newGroupName))
		GroupManager::instance()->byName(newGroupName);
}

void GroupTabBar::groupProperties()
{
	if (!currentGroup)
		return;

	(new GroupPropertiesWindow(currentGroup, Core::instance()->kaduWindow()))->show();
}

void GroupTabBar::addToGroup()
{
	if (!currentGroup)
		return;

	foreach (Buddy buddy, currentBuddies)
		buddy.addToGroup(currentGroup);
}

void GroupTabBar::moveToGroup()
{
	if (!currentGroup)
		return;

	foreach (Buddy buddy, currentBuddies)
	{
		buddy.removeFromGroup(GroupManager::instance()->byUuid(tabData(currentIndex()).toString()));
		buddy.addToGroup(currentGroup);

		Filter->refresh();
	}
}

 void GroupTabBar::saveGroupTabsPosition()
 {
	Group group;
	for (int i = 0; i < count(); ++i)
	{
		group = GroupManager::instance()->byUuid(tabData(i).toString());
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
		Filter->setAllGroupShown(true);
		Group group;
		for (int i = 0; i < count(); ++i)
		{
			group = GroupManager::instance()->byUuid(tabData(i).toString());
			if (!group)
			{
				setCurrentIndex(i);
				break;
			}
		}
		Filter->refresh();
		setVisible(false);
		return;
	}

	setVisible(true);

	bool show = config_file.readBoolEntry("Look", "ShowGroupAll", true);

	int autoGroupOldPosition = -1;

	for (int i = 0; i < count(); ++i)
		if (tabData(i).toString() == "AutoTab")
		{
			autoGroupOldPosition = i;
			break;
		}


	if (showAllGroup != show)
	{
		if (show)
		{
			config_file.writeEntry("Look", "UngroupedGroupTabPosition", autoGroupOldPosition);
			AutoGroupTabPosition = 	config_file.readNumEntry("Look", "AllGroupTabPosition", -1);
			setTabText(autoGroupOldPosition, tr("All"));
			setTabIcon(autoGroupOldPosition, IconsManager::instance()->iconByPath("x-office-address-book"));
		}
		else
		{
			config_file.writeEntry("Look", "AllGroupTabPosition", autoGroupOldPosition);
			AutoGroupTabPosition = 	config_file.readNumEntry("Look", "UngroupedGroupTabPosition", -1);
			setTabText(autoGroupOldPosition, tr("Ungrouped"));
			setTabIcon(autoGroupOldPosition, QIcon(QString()));
		}

		moveTab(autoGroupOldPosition, AutoGroupTabPosition);
	}

	showAllGroup = show;
	Filter->setAllGroupShown(showAllGroup);

	if (AutoGroupTabPosition == currentIndex())
			Filter->refresh();
}
