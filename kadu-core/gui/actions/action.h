/*
 * %kadu copyright begin%
 * Copyright 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#ifndef ACTION_H
#define ACTION_H

#include <QtCore/QMultiMap>
#include <QtGui/QAction>
#include <QtGui/QIcon>

#include "configuration/configuration-aware-object.h"

#include "exports.h"

class ActionDataSource;
class ActionDescription;
class Buddy;
class BuddySet;
class Chat;
class Contact;
class ContactSet;
class MainWindow;

class KADUAPI Action : public QAction
{
	Q_OBJECT

	ActionDescription *Description;
	ActionDataSource *DataSource;

	QIcon Icon;

private slots:
	void changedSlot();
	void hoveredSlot();
	void triggeredSlot(bool checked);

public:
	Action(ActionDescription *description, ActionDataSource *dataSource, QObject *parent);
	virtual ~Action();

	Contact contact();
	ContactSet contacts();

	Buddy buddy();
	BuddySet buddies();

	Chat chat();
	
	ActionDataSource *dataSource();	

public slots:
	void checkState();
	void updateIcon();

signals:
	void changed(QAction *action);
	void hovered(QAction *action);
	void toggled(QAction *action, bool checked);
	void triggered(QAction *action, bool checked = false);

};

void disableEmptyContacts(Action *action);
void disableNoChat(Action *action);

#endif // ACTION_H
