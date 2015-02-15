/*
 * %kadu copyright begin%
 * Copyright 2014 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2012, 2013, 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include <QtWidgets/QMenu>

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
