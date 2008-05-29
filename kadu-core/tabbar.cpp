/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <QApplication>
#include <QDragEnterEvent>
#include <QInputDialog>

#include "debug.h"
#include "groups_manager.h"
#include "userbox.h"
#include "userinfo.h"

#include "tabbar.h"

KaduTabBar::KaduTabBar(QWidget *parent)
	: QTabBar(parent)
{
	kdebugf();
	setSizePolicy(QSizePolicy(QSizePolicy::Maximum, QSizePolicy::Preferred));
	setAcceptDrops(true);
}

void KaduTabBar::dragEnterEvent(QDragEnterEvent *e)
{
	kdebugf();
	e->accept(UlesDrag::canDecode(e) &&
		dynamic_cast<UserBox*>(e->source()));
	kdebugf2();
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

		if (UserInfo::acceptableGroupName(text))
			group = text;
	}
	while (group.isEmpty());

	return group;
}

void KaduTabBar::dropEvent(QDropEvent *e)
{
	kdebugf();

	QStringList ules;
	if (!dynamic_cast<UserBox*>(e->source()) || !UlesDrag::decode(e, ules))
		return;

	QApplication::setOverrideCursor(QCursor(Qt::ArrowCursor));
	QString group;
	int tabIndex = tabAt(e->pos());

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

	if (group == GroupsManager::tr("All"))
		group = QString::null;

	currentGroup = group;
	currentUles = ules;

	QMenu menu(this);
	menu.addAction(tr("Add to group %1").arg(group), this, SLOT(addToGroup()));
	if (tabText(currentIndex()) != GroupsManager::tr("All"))
		menu.addAction(tr("Move to group %1").arg(group), this, SLOT(moveToGroup()));
	menu.exec(QCursor::pos());

	currentGroup = QString::null;

	userlist->writeToConfig();

	QApplication::restoreOverrideCursor();

	kdebugf2();
}

void KaduTabBar::addToGroup()
{
	if (currentGroup.isEmpty())
		return;

	foreach(QString ule, currentUles)
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

	foreach(QString ule, currentUles)
		userlist->byAltNick(ule).setData("Groups", groups);
}
