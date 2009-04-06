/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <QtGui/QApplication>
#include <QtGui/QDragEnterEvent>
#include <QtGui/QInputDialog>
#include <QtGui/QMenu>

#include "contacts/contact-list-mime-data-helper.h"
#include "contacts/group.h"
#include "contacts/group-manager.h"
#include "contacts/model/filter/group-contact-filter.h"
#include "core/core.h"
#include "gui/windows/group-properties-window.h"
#include "gui/windows/kadu-window.h"

#include "config_file.h"
#include "debug.h"
#include "icons_manager.h"
#include "message_box.h"

#include "group-tab-bar.h"

GroupTabBar::GroupTabBar(QWidget *parent)
	: QTabBar(parent)
{
	Filter = new GroupContactFilter(this);

	setSizePolicy(QSizePolicy(QSizePolicy::Minimum, QSizePolicy::Expanding));
 	setAcceptDrops(true);
	setDrawBase(false);

	setShape(QTabBar::RoundedWest);
	addTab(icons_manager->loadIcon("PersonalInfo"), tr("All"));
	setTabData(0, "AllTab");

	setFont(QFont(config_file.readFontEntry("Look", "UserboxFont").family(),
			config_file.readFontEntry("Look", "UserboxFont").pointSize(), QFont::Bold));
	setIconSize(QSize(16, 16));

	foreach (const Group *group, GroupManager::instance()->groups())
		addGroup(group);

	connect(this, SIGNAL(currentChanged(int)), this, SLOT(currentChangedSlot(int)));

	connect(GroupManager::instance(), SIGNAL(groupAdded(Group *)), this, SLOT(groupAdded(Group *)));
	connect(GroupManager::instance(), SIGNAL(groupAboutToBeRemoved(Group *)), this, SLOT(groupRemoved(Group *)));

	configurationUpdated();

	setCurrentIndex(config_file.readNumEntry("Look", "CurrentGroupTab", 0));
}

GroupTabBar::~GroupTabBar()
{
	config_file.writeEntry("Look", "CurrentGroupTab", currentIndex());
}

void GroupTabBar::addGroup(const Group *group)
{
	int index = addTab(group->name());
	setTabData(index, group->uuid().toString());
	connect(group, SIGNAL(appearanceChanged(const Group *)), this, SLOT(groupAppearanceChanged(const Group *)));
	connect(group, SIGNAL(nameChanged(const Group *)), this, SLOT(groupNameChanged(const Group *)));
	connect(group, SIGNAL(showInAllChanged()), this, SLOT(showInAllGroupChanged()));

	groupAppearanceChanged(group);
}

void GroupTabBar::currentChangedSlot(int index)
{
	Group *group = GroupManager::instance()->byUuid(tabData(index).toString());
	emit currentGroupChanged(group);
	Filter->setGroup(group);
}

void GroupTabBar::groupAdded(Group *group)
{
	QString groupUuid = group->uuid().toString();
	for (int i = 0; i < count(); ++i)
		if (tabData(i).toString() == groupUuid) //group is already in tabbar
			return;
	addGroup(group);
}

void GroupTabBar::groupRemoved(Group *group)
{
	QString groupUuid = group->uuid().toString();
	for (int i = 0; i < count(); ++i)
		if (tabData(i).toString() == groupUuid)
		{
			removeTab(i);
			return;
		}
}

void GroupTabBar::groupAppearanceChanged(const Group *group)
{	
	QString groupUuid = group->uuid().toString();
	for (int i = 0; i < count(); ++i)
		if (tabData(i).toString() == groupUuid)
		{
			setTabIcon(i, QIcon(group->showIcon() ? group->icon() : ""));
			setTabText(i, group->showName() ? group->name() : "");
			break;
		}
			
}

void GroupTabBar::groupNameChanged(const Group *group)
{
	if (!group->showName())
		return;

	QString groupUuid = group->uuid().toString();
	for (int i = 0; i < count(); ++i)
		if (tabData(i).toString() == groupUuid)
		{
			setTabText(i, group->name());
			break;
		}
}

void GroupTabBar::showInAllGroupChanged()
{
    if (tabData(currentIndex()).toString() == "AllTab")
		Filter->refresh();
}

void GroupTabBar::contextMenuEvent(QContextMenuEvent *event)
{
	int tabIndex = tabAt(event->pos());

	if (tabIndex != -1)
		currentGroup = GroupManager::instance()->byUuid(tabData(tabIndex).toString());

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

	ContactList contacts = ContactListMimeDataHelper::fromMimeData(event->mimeData());

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
				QString::null, &ok);

			if (!ok)
				return;

			ok = !newGroupName.isEmpty() && GroupManager::instance()->acceptableGroupName(newGroupName);
		}
		while (!ok);

		Group *group = GroupManager::instance()->byName(newGroupName);

		foreach (Contact contact, contacts)
			contact.addToGroup(group);

		QApplication::restoreOverrideCursor();

		return;
	}
	else
		currentGroup = GroupManager::instance()->byUuid(tabData(tabIndex).toString());

	currentContacts = contacts;

	QMenu menu(this);
	if (currentGroup)
	{
		menu.addAction(tr("Move to group %1").arg(currentGroup->name()), this, SLOT(moveToGroup()))
				->setEnabled(tabData(currentIndex()).toString() != "AllTab");
		menu.addAction(tr("Add to group %1").arg(currentGroup->name()), this, SLOT(addToGroup()));
	}

	menu.exec(QCursor::pos());

	QApplication::restoreOverrideCursor();

	kdebugf2();
}

void GroupTabBar::addBuddy()
{

}


void GroupTabBar::renameGroup()
{
	if (!currentGroup)
		return;
	bool ok;
	QString text = QInputDialog::getText(this, tr("Rename Group"),
				tr("Please enter a new name for this group"), QLineEdit::Normal,
				QString::null, &ok);

	if (ok && !text.isEmpty() && GroupManager::instance()->acceptableGroupName(text))
		currentGroup->setName(text);
}

void GroupTabBar::deleteGroup()
{
	if (currentGroup && MessageBox::ask(tr("Selected group:\n%0 will be deleted. Are you sure?").arg(currentGroup->name()), "Warning", Core::instance()->kaduWindow()))
		GroupManager::instance()->removeGroup(currentGroup->uuid().toString());
}

void GroupTabBar::createNewGroup()
{
	bool ok;
	QString newGroupName = QInputDialog::getText(this, tr("New Group"),
				tr("Please enter the name for the new group:"), QLineEdit::Normal,
				QString::null, &ok);

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

	foreach(Contact contact, currentContacts)
		contact.addToGroup(currentGroup);
}

void GroupTabBar::moveToGroup()
{
	if (!currentGroup)
		return;

	QStringList groups;

	foreach(Contact contact, currentContacts)
	{
		contact.removeFromGroup(GroupManager::instance()->byUuid(tabData(currentIndex()).toString()));
		contact.addToGroup(currentGroup);

		Filter->refresh();
	}
}
//TODO 0.6.6:
void GroupTabBar::configurationUpdated()
{
	bool show = config_file.readBoolEntry("Look", "ShowGroupAll", true);
	Filter->setAllGroupShown(show);

	for (int i = 0; i < count(); ++i)
		if (tabData(i).toString() == "AllTab")
		{
			if (show && tabText(i) != tr("All"))
			{
				setTabText(i, tr("All"));
				if (tabData(currentIndex()).toString() == "AllTab")
					Filter->refresh();
			}
			else if (!show && tabText(i) == tr("All"))
			{
				setTabText(i, tr("Ungrouped"));
				if (tabData(currentIndex()).toString() == "AllTab")
					Filter->refresh();
			}
			return;
		}
}
