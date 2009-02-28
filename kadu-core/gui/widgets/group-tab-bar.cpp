/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "contacts/group.h"
#include "contacts/group-manager.h"

#include "contacts/model/filter/group-contact-filter.h"

#include "config_file.h"
#include "icons_manager.h"

#include "group-tab-bar.h"

GroupTabBar::GroupTabBar(QWidget *parent)
	: QTabBar(parent)
{
	Filter = new GroupContactFilter(this);

	setSizePolicy(QSizePolicy(QSizePolicy::Maximum, QSizePolicy::Preferred));
// 	setAcceptDrops(true);

	setShape(QTabBar::RoundedWest);
	addTab(icons_manager->loadIcon("PersonalInfo"), tr("All"));
	setFont(QFont(config_file.readFontEntry("Look", "UserboxFont").family(),
			config_file.readFontEntry("Look", "UserboxFont").pointSize(), QFont::Bold));
	setIconSize(QSize(16, 16));

	foreach (const Group *group, GroupManager::instance()->groups())
		addGroup(group);

	connect(this, SIGNAL(currentChanged(int)), this, SLOT(currentChangedSlot(int)));

	connect(GroupManager::instance(), SIGNAL(groupAdded(Group *)), this, SLOT(groupAdded(Group *)));

	setCurrentIndex(config_file.readNumEntry("Look", "CurrentGroupTab", 0));
}

GroupTabBar::~GroupTabBar()
{
	config_file.writeEntry("Look", "CurrentGroupTab", currentIndex());
}

void GroupTabBar::addGroup(const Group *group)
{
	int index = addTab(QIcon(group->icon()), group->name());
	setTabData(index, group->uuid().toString());
	connect(group, SIGNAL(iconChanged(const Group *)), this, SLOT(groupIconChanged(const Group *)));
	connect(group, SIGNAL(nameChanged(const Group *)), this, SLOT(groupNameChanged(const Group *)));
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

void GroupTabBar::groupIconChanged(const Group *group)
{	
	QString groupUuid = group->uuid().toString();
	for (int i = 0; i < count(); ++i)
		if (tabData(i).toString() == groupUuid)
		{
			setTabIcon(i, QIcon(group->icon()));
			break;
		}
			
}

void GroupTabBar::groupNameChanged(const Group *group)
{
	QString groupUuid = group->uuid().toString();
	for (int i = 0; i < count(); ++i)
		if (tabData(i).toString() == groupUuid)
		{
			setTabText(i, group->name());
			break;
		}
}

/*

void KaduTabBar::dragEnterEvent(QDragEnterEvent *event)
{
	if (event->mimeData()->hasFormat("application/x-kadu-ules"))
		event->acceptProposedAction();
}

QString KaduTabBar::getNewGroupNameFromUser(bool *ok)
{
	QString group;
	QString text;

	do
	{
		text = QInputDialog::getText(tr("Add new group"), tr("Name of new group:"), QLineEdit::Normal, text, ok);
		if (!*ok)
			return QString::null;

		if (ContactDataWindow::acceptableGroupName(text))
			group = text;
	}
	while (group.isEmpty());

	return group;
}

void KaduTabBar::dropEvent(QDropEvent *event)
{
	kdebugf();

	QStringList ules;
	if (!event->mimeData()->hasFormat("application/x-kadu-ules"))
		return;

	event->acceptProposedAction();

	QApplication::setOverrideCursor(QCursor(Qt::ArrowCursor));
	QString group;
	int tabIndex = tabAt(event->pos());

	if (tabIndex == -1)
	{
		bool ok;
		group = getNewGroupNameFromUser(&ok);
		if (!ok)
		{
			QApplication::restoreOverrideCursor();
			return;
		}
	}
	else
		group = tabText(tabIndex);

// 	if (group == GroupsManagerOld::tr("All"))
// 		group = QString::null;

	currentGroup = group;
	currentUles = ules;

	QMenu menu(this);
	menu.addAction(tr("Add to group %1").arg(group), this, SLOT(addToGroup()));
	if (tabText(currentIndex()) != GroupsManagerOld::tr("All"))
		menu.addAction(tr("Move to group %1").arg(group), this, SLOT(moveToGroup()));
	menu.exec(QCursor::pos());

	currentGroup = QString::null;

// TODO: 0.6.6
// 	userlist->writeToConfig();

	QApplication::restoreOverrideCursor();

	kdebugf2();
}

void KaduTabBar::addToGroup()
{
	if (currentGroup.isEmpty())
		return;

	foreach(const QString &ule, currentUles)
	{
		UserListElement user = userlist->byAltNick(ule);
		QStringList userGroups = user.data("Groups").toStringList();
		if (!userGroups.contains(currentGroup))
		{
			userGroups.append(currentGroup);
			user.setData("Groups", userGroups);
		}
	}
}

void KaduTabBar::moveToGroup()
{
	QStringList groups;
	if (!currentGroup.isEmpty())
		groups.append(currentGroup);

	foreach(const QString &ule, currentUles)
		userlist->byAltNick(ule).setData("Groups", groups);
}*/
