/*
 * %kadu copyright begin%
 * Copyright 2011 SÅawomir StÄpieÅ (s.stepien@interia.pl)
 * Copyright 2011 Slawomir Stepien (s.stepien@interia.pl)
 * Copyright 2011, 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#ifndef CUSTOM_ITEM_MENU_MANAGER_H
#define CUSTOM_ITEM_MENU_MANAGER_H

#include <QtCore/QObject>

#include "gui/widgets/custom-input-menu-item.h"
#include "exports.h"

class QMenu;
class QWidget;

class ActionContext;
class ActionDescription;

class KADUAPI CustomInputMenuManager : public QObject
{
	Q_OBJECT
	Q_DISABLE_COPY(CustomInputMenuManager)

	static CustomInputMenuManager *Instance;

	QList<CustomInputMenuItem> InputContextMenu;
	bool InputContextMenuSorted;

	CustomInputMenuManager();

	void sortInputContextMenu();

public:
	static CustomInputMenuManager * instance();

	void addActionDescription(ActionDescription *actionDescription, CustomInputMenuItem::CustomInputMenuCategory category, int priority);
	void removeActionDescription(ActionDescription *actionDescription);

	QMenu * menu(QWidget *parent);
};

#endif // CUSTOM_ITEM_MENU_MANAGER_H
