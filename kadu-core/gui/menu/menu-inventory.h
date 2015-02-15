/*
 * %kadu copyright begin%
 * Copyright 2012 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2012, 2013 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#ifndef MENU_INVENTORY_H
#define MENU_INVENTORY_H

#include <QtCore/QObject>

#include "exports.h"
#include "kadu-menu.h"

class QMenu;
class QWidget;

class ActionContext;
class ActionDescription;
class Contact;
class ProtocolMenuManager;

class KADUAPI MenuInventory : public QObject
{
	Q_OBJECT
	Q_DISABLE_COPY(MenuInventory)

	static MenuInventory *Instance;

	QMap<QString, KaduMenu *> Menus;
	QList<ProtocolMenuManager *> ProtocolMenuManagers;

	MenuInventory();

public:
	static MenuInventory * instance();

	KaduMenu * menu(const QString &category);

	void registerProtocolMenuManager(ProtocolMenuManager *manager);
	void unregisterProtocolMenuManager(ProtocolMenuManager *manager);

	QList<ProtocolMenuManager *> & protocolMenuManagers() { return ProtocolMenuManagers; }

};

#endif // MENU_INVENTORY_H
