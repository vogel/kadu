/*
 * %kadu copyright begin%
 * Copyright 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009 Michał Podsiadlik (michal@kadu.net)
 * Copyright 2009, 2010, 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2009 Longer (longer89@gmail.com)
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

#ifndef TALKABLE_MENU_MANAGER_H
#define TALKABLE_MENU_MANAGER_H

#include <QtCore/QObject>

#include "gui/widgets/talkable-menu-item.h"
#include "exports.h"

class QMenu;
class QWidget;

class ActionContext;
class ActionDescription;
class Contact;

class KADUAPI TalkableMenuManager : public QObject
{
	Q_OBJECT
	Q_DISABLE_COPY(TalkableMenuManager)

	static TalkableMenuManager *Instance;

	QList<TalkableMenuItem> ContexMenu;
	bool ContexMenuSorted;

	QList<TalkableMenuItem> Actions;
	bool ActionsSorted;

	TalkableMenuManager();

	void sortContexMenu();
	void sortActions();

public:
	static TalkableMenuManager * instance();

	void addActionDescription(ActionDescription *actionDescription, TalkableMenuItem::MenuCategory category, int priority);
	void removeActionDescription(ActionDescription *actionDescription);

	void addListActionDescription(ActionDescription *actionDescription, TalkableMenuItem::MenuCategory category, int priority);
	void removeListActionDescription(ActionDescription *actionDescription);

	QMenu * menu(QWidget *parent, ActionContext *actionContext);

};

#endif // TALKABLE_MENU_MANAGER_H
