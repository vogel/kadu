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

#ifndef ADD_ROOM_CHAT_WINDOW_H
#define ADD_ROOM_CHAT_WINDOW_H

#include <QtWidgets/QDialog>

#include "buddies/buddy-set.h"

class QLabel;
class QLineEdit;

class AccountsComboBox;
class AccountTalkableFilter;
class BuddyListModel;
class Chat;
class CheckableBuddiesProxyModel;
class ModelChain;

class AddRoomChatWindow : public QDialog
{
	Q_OBJECT

	AccountsComboBox *AccountCombo;
	QLineEdit *DisplayNameEdit;
	QLineEdit *RoomEdit;
	QLineEdit *NickEdit;
	QLineEdit *PasswordEdit;
	QLabel *ErrorLabel;
	QPushButton *AddButton;
	QPushButton *StartButton;

	void createGui();
	void displayErrorMessage(const QString &message);

	Chat computeChat() const;

private slots:
	void validateData();

public:
	explicit AddRoomChatWindow(QWidget *parent = 0);
	virtual ~AddRoomChatWindow();

public slots:
	void accept();
	void start();

};

#endif // ADD_ROOM_CHAT_WINDOW_H
