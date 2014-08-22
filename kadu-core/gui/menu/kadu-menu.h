/*
 * %kadu copyright begin%
 * Copyright 2012 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2012, 2013, 2014 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2013 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#ifndef KADU_MENU_H
#define KADU_MENU_H

#include <QtCore/QList>
#include <QtCore/QMap>
#include <QtWidgets/QMenu>

#include "exports.h"

class ActionContext;
class ActionDescription;
class MenuItem;

class KADUAPI KaduMenu : public QObject
{
	Q_OBJECT

public:
	enum MenuSection
	{
		SectionConfig,
		SectionRecentChats,
		SectionMiscTools,
		SectionQuit,
		SectionBuddies,
		SectionOpenChat,
		SectionBuddyListFilters,
		SectionTools,
		SectionHelp,
		SectionGetInvolved,
		SectionAbout,
		SectionChat,
		SectionSend,
		SectionActions,
		SectionView,
		SectionManagement,
	};

	QString Category;
	QList<MenuItem *> Items;
	bool IsSorted;

	QList<QMenu *> Menus;

	void sort();
	ActionContext * getActionContext();

	void appendTo(QMenu *menu, ActionContext *context = 0);
	void applyTo(QMenu *menu, ActionContext *context = 0);

private slots:
	void menuDestroyed(QObject *object);

	void updateGuiMenuSlot();

public:
	explicit KaduMenu(const QString &category, KaduMenu *parent = 0);
	virtual ~KaduMenu();

	void attachToMenu(QMenu *menu);
	void detachFromMenu(QMenu *menu);

	bool empty() const;

	KaduMenu * addAction(ActionDescription *actionDescription, KaduMenu::MenuSection section, int priority = 0);
	KaduMenu * removeAction(ActionDescription *actionDescription);
	void updateGuiMenuLater();
	void update();

	static bool lessThan(const MenuItem *a, const MenuItem *b);

};

#endif // KADU_MENU_H
