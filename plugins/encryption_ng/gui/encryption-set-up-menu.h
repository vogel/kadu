/*
 * %kadu copyright begin%
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2004 Adrian Smarzewski (adrian@kadu.net)
 * Copyright 2007, 2008, 2009, 2010, 2011, 2012 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2004, 2006 Marcin Ślusarz (joi@kadu.net)
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

#ifndef ENCRYPTION_SET_UP_MENU_H
#define ENCRYPTION_SET_UP_MENU_H

#include <QtGui/QMenu>

class Action;

class EncryptionSetUpMenu : public QMenu
{
	Q_OBJECT

	Action *MenuAction;

	QActionGroup *EncryptorsGroup;

private slots:
	void aboutToShowSlot();
	void encryptionMethodSelected(QAction *selectedAction);

public:
	explicit EncryptionSetUpMenu(Action *action, QWidget *parent = 0);
	virtual ~EncryptionSetUpMenu();

};

#endif // ENCRYPTION_SET_UP_MENU_H
