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
		addTab(group->name());

	connect(this, SIGNAL(currentChanged(int)), this, SLOT(currentChangedSlot(int)));
}

void GroupTabBar::currentChangedSlot(int index)
{
	Group *group = GroupManager::instance()->byName(tabText(index), false);
	emit currentGroupChanged(group);
	Filter->setGroup(group);
}


// 	if (GroupBar)
// 		config_file.writeEntry("Look", "CurrentGroupTab", GroupBar->currentIndex());


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
