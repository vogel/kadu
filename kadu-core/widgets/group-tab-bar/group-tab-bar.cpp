/*
 * %kadu copyright begin%
 * Copyright 2014 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2013, 2014, 2015 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "group-filter-tab-data.h"
#include "group-tab-bar.h"

#include "buddies/buddy-list-mime-data-service.h"
#include "buddies/group-manager.h"
#include "buddies/group.h"
#include "chat/chat-list-mime-data-service.h"
#include "core/core.h"
#include "core/injected-factory.h"
#include "icons/icons-manager.h"
#include "icons/kadu-icon.h"
#include "talkable/filter/group-filter.h"
#include "windows/add-buddy-window.h"
#include "windows/group-edit-window.h"
#include "windows/kadu-dialog.h"
#include "windows/kadu-window-service.h"
#include "windows/kadu-window.h"
#include "windows/message-dialog.h"
#include "debug.h"

#include <QtCore/QMimeData>
#include <QtGui/QDragEnterEvent>
#include <QtWidgets/QApplication>
#include <QtWidgets/QInputDialog>
#include <QtWidgets/QMenu>

GroupTabBar::GroupTabBar(QWidget *parent) :
		QTabBar(parent)
{
	setSizePolicy(QSizePolicy(QSizePolicy::Minimum, QSizePolicy::Expanding));

 	setAcceptDrops(true);
	setDrawBase(false);
	setMovable(true);

	setShape(QTabBar::RoundedWest);
	setIconSize(QSize(16, 16));

	connect(this, SIGNAL(currentChanged(int)), this, SLOT(currentChangedSlot(int)));
}

GroupTabBar::~GroupTabBar()
{
}

void GroupTabBar::setBuddyListMimeDataService(BuddyListMimeDataService *buddyListMimeDataService)
{
	m_buddyListMimeDataService = buddyListMimeDataService;
}

void GroupTabBar::setChatListMimeDataService(ChatListMimeDataService *chatListMimeDataService)
{
	m_chatListMimeDataService = chatListMimeDataService;
}

void GroupTabBar::setGroupManager(GroupManager *groupManager)
{
	m_groupManager = groupManager;
}

void GroupTabBar::setIconsManager(IconsManager *iconsManager)
{
	m_iconsManager = iconsManager;
}

void GroupTabBar::setInjectedFactory(InjectedFactory *injectedFactory)
{
	m_injectedFactory = injectedFactory;
}

void GroupTabBar::setKaduWindowService(KaduWindowService *kaduWindowService)
{
	m_kaduWindowService = kaduWindowService;
}

void GroupTabBar::init()
{
	connect(m_groupManager, SIGNAL(groupAdded(Group)), this, SLOT(addGroup(Group)));
	connect(m_groupManager, SIGNAL(groupAboutToBeRemoved(Group)), this, SLOT(removeGroup(Group)));
	connect(m_groupManager, SIGNAL(groupUpdated(Group)), this, SLOT(updateGroup(Group)));
}

void GroupTabBar::setInitialConfiguration(GroupTabBarConfiguration configuration)
{
	foreach (const auto &groupFilter, configuration.groupFilters())
		insertGroupFilter(count(), groupFilter);

	foreach (const Group &group, m_groupManager->items())
		addGroup(group);

	setConfiguration(configuration);
}

void GroupTabBar::setConfiguration(GroupTabBarConfiguration configuration)
{
	m_groupTabBarConfiguration = configuration;

	setVisible(m_groupTabBarConfiguration.displayGroupTabs());

	if (m_groupTabBarConfiguration.showGroupTabEverybody())
		insertGroupFilter(0, GroupFilter(GroupFilterEverybody));
	else
		removeGroupFilter(GroupFilter(GroupFilterEverybody));

	updateUngrouppedTab();

	if (m_groupTabBarConfiguration.displayGroupTabs())
	{
		if (currentIndex() == configuration.currentGroupTab())
			currentChangedSlot(configuration.currentGroupTab());
		else
			setCurrentIndex(configuration.currentGroupTab());
	}
	else
	{
		m_groupFilter = GroupFilter{GroupFilterEverybody};
		emit currentGroupFilterChanged(m_groupFilter);
	}
}

void GroupTabBar::updateUngrouppedTab()
{
	if (shouldShowUngrouppedTab())
		insertGroupFilter(count(), GroupFilter(GroupFilterUngroupped));
	else
		removeGroupFilter(GroupFilter(GroupFilterUngroupped));
}

bool GroupTabBar::shouldShowUngrouppedTab() const
{
	return m_groupTabBarConfiguration.alwaysShowGroupTabUngroupped()
			? true
			: !m_groupTabBarConfiguration.showGroupTabEverybody();
}

GroupTabBarConfiguration GroupTabBar::configuration()
{
	m_groupTabBarConfiguration.setGroupFilters(groupFilters()); // update only if needed
	return m_groupTabBarConfiguration;
}

Group GroupTabBar::groupAt(int index) const
{
	return groupFilterAt(index).group();
}

GroupFilter GroupTabBar::groupFilter() const
{
	if (m_groupTabBarConfiguration.displayGroupTabs())
		return groupFilterAt(currentIndex());
	else
		return GroupFilter{GroupFilterEverybody};
}

GroupFilter GroupTabBar::groupFilterAt(int index) const
{
	return tabData(index).value<GroupFilter>();
}

QVector<GroupFilter> GroupTabBar::groupFilters() const
{
	auto tabsCount = count();
	auto result = QVector<GroupFilter>(tabsCount);

	for (auto i = 0; i < tabsCount; i++)
		result.append(tabData(i).value<GroupFilter>());

	return result;
}

void GroupTabBar::currentChangedSlot(int index)
{
	m_groupTabBarConfiguration.setCurrentGroupTab(index);
	if (m_groupTabBarConfiguration.displayGroupTabs())
		m_groupFilter = groupFilterAt(index);
	else
		m_groupFilter = GroupFilter{GroupFilterEverybody};

	emit currentGroupFilterChanged(m_groupFilter);

}

void GroupTabBar::contextMenuEvent(QContextMenuEvent *event)
{
	auto group = groupAt(tabAt(event->pos()));

	QMenu menu;

	QAction *addBuddyAction = menu.addAction(tr("Add Buddy"), this, SLOT(addBuddy()));
	addBuddyAction->setEnabled(group);
	addBuddyAction->setData(group);

	menu.addAction(tr("Add Group"), this, SLOT(createNewGroup()));

	QAction *deleteGroupAction = menu.addAction(tr("Delete Group"), this, SLOT(deleteGroup()));
	deleteGroupAction->setEnabled(group);
	deleteGroupAction->setData(group);

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

	auto buddies = m_buddyListMimeDataService->fromMimeData(event->mimeData());
	auto chats = m_chatListMimeDataService->fromMimeData(event->mimeData());

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

			ok = m_groupManager->acceptableGroupName(newGroupName);
		}
		while (!ok);

		Group group = m_groupManager->byName(newGroupName);

		foreach (const Buddy &buddy, buddies)
			buddy.addToGroup(group);
		foreach (const Chat &chat, chats)
			chat.addToGroup(group);

		QApplication::restoreOverrideCursor();

		return;
	}

	auto clickedGroup = groupAt(tabIndex);

	DNDBuddies = buddies;
	DNDChats = chats;

	if (clickedGroup)
	{
		QMenu menu;
		if (m_groupFilter.filterType() == GroupFilterRegular)
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

	auto addBuddyWindow = m_injectedFactory->makeInjected<AddBuddyWindow>(m_kaduWindowService->kaduWindow());
	addBuddyWindow->setGroup(action->data().value<Group>());
	addBuddyWindow->show();
}

void GroupTabBar::deleteGroup()
{
	QAction *action = qobject_cast<QAction *>(sender());
	if (!action)
		return;

	const Group &group = action->data().value<Group>();
	if (!group)
		return;

	MessageDialog *dialog = MessageDialog::create(m_iconsManager->iconByPath(KaduIcon("dialog-warning")),
						      tr("Delete group"),
						      tr("Group <i>%0</i> will be deleted, but without buddies. Are you sure?").arg(group.name()),
						      m_kaduWindowService->kaduWindow());
	dialog->addButton(QMessageBox::Yes, tr("Delete group"));
	dialog->addButton(QMessageBox::No, tr("Cancel"));

	if (group && dialog->ask())
		m_groupManager->removeItem(group);
}

void GroupTabBar::createNewGroup()
{
	auto editWindow = m_injectedFactory->makeInjected<GroupEditWindow>(Group::null, m_kaduWindowService->kaduWindow());
	editWindow->show();
}

void GroupTabBar::groupProperties()
{
	QAction *action = qobject_cast<QAction *>(sender());
	if (!action)
		return;

	auto group = action->data().value<Group>();
	if (!group)
		return;

	auto editWindow = m_injectedFactory->makeInjected<GroupEditWindow>(group, m_kaduWindowService->kaduWindow());
	editWindow->show();
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

	const Group &removeFromGroup = m_groupManager->byUuid(tabData(currentIndex()).toString());
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

int GroupTabBar::indexOf(GroupFilter groupFilter)
{
	auto tabsCount = count();
	for (auto i = 0; i < tabsCount; i++)
		if (groupFilter == tabData(i).value<GroupFilter>())
			return i;
	return -1;
}

void GroupTabBar::addGroup(Group group)
{
	insertGroupFilter(count(), GroupFilter(group));
}

void GroupTabBar::insertGroupFilter(int index, GroupFilter groupFilter)
{
	if (indexOf(groupFilter) >= 0)
		return;

	auto newTabIndex = insertTab(index, QString());
	setTabData(newTabIndex, QVariant::fromValue(groupFilter));
	updateTabData(newTabIndex, groupFilter);
}

void GroupTabBar::removeGroup(Group group)
{
	removeGroupFilter(GroupFilter(group));
}

void GroupTabBar::removeGroupFilter(GroupFilter groupFilter)
{
	auto index = indexOf(groupFilter);
	if (index >= 0)
		removeTab(index);
}

void GroupTabBar::updateGroup(Group group)
{
	auto groupFilter = GroupFilter(group);
	auto index = indexOf(groupFilter);
	if (index >= 0)
		updateTabData(index, groupFilter);
}

void GroupTabBar::updateTabData(int tabIndex, GroupFilter groupFilter)
{
	auto tabData = GroupFilterTabData(groupFilter);
	setTabText(tabIndex, tabData.tabName());
	setTabIcon(tabIndex, tabData.tabIcon(m_iconsManager));
}

#include "moc_group-tab-bar.cpp"
