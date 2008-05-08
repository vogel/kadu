/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <QApplication>
#include <QInputDialog>

#include "debug.h"
#include "groups_manager.h"
#include "misc.h"
#include "userbox.h"
#include "userinfo.h"

#include "tabbar.h"

KaduTabBar::KaduTabBar(QWidget *parent)	: QTabBar(parent)
{
	kdebugf();
	setSizePolicy(QSizePolicy(QSizePolicy::Maximum, QSizePolicy::Preferred));
	setAcceptDrops(true);
}

void KaduTabBar::dragEnterEvent(QDragEnterEvent* e)
{
	kdebugf();
	e->accept(UlesDrag::canDecode(e) &&
		dynamic_cast<UserBox*>(e->source()));
	kdebugf2();
}

void KaduTabBar::dropEvent(QDropEvent* e)
{
	kdebugf();

	QStringList ules;
	if (dynamic_cast<UserBox*>(e->source()) && UlesDrag::decode(e, ules))
	{
		QApplication::setOverrideCursor(QCursor(Qt::ArrowCursor));
		QString group;
		int tabIndex = tabAt(e->pos());
		if (tabIndex!=-1)
			group = tabText(tabIndex);
		else
		{
			bool ok;
			QString text;
			do
			{
				text = QInputDialog::getText(tr("Add new group"), tr("Name of new group:"), QLineEdit::Normal, text, &ok);
				if (!ok)
				{
					QApplication::restoreOverrideCursor();
					return;
				}
				if (UserInfo::acceptableGroupName(text))
					group = text;
			}
			while (group.isEmpty());
		}
		if (group == GroupsManager::tr("All"))
			group = QString::null;

		Q3PopupMenu menu(this);
		menu.insertItem(tr("Add to group %1").arg(group), 2);

		if (tabText(currentIndex()) != GroupsManager::tr("All"))
			menu.insertItem(tr("Move to group %1").arg(group), 1);

		int menuret = -1;
		if (group.isEmpty() || (menuret = menu.exec(mapToGlobal(e->pos()))) == 1)
		{
			QStringList groups;
			if (!group.isEmpty())
				groups.append(group);
			CONST_FOREACH(ule, ules)
			{
				userlist->byAltNick(*ule).setData("Groups", groups);
			}
		}
		else if (menuret == 2)
		{
			CONST_FOREACH(ule, ules)
			{
				UserListElement user = userlist->byAltNick(*ule);
				QStringList userGroups = user.data("Groups").toStringList();
				if (!userGroups.contains(group))
				{
					userGroups.append(group);
					user.setData("Groups", userGroups);
				}
			}
		}

		// too slow, we need to do something about that
		userlist->writeToConfig();

		QApplication::restoreOverrideCursor();
	}
	kdebugf2();
}
