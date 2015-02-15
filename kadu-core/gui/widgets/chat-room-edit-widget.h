/*
 * %kadu copyright begin%
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

#ifndef CHAT_ROOM_EDIT_WIDGET_H
#define CHAT_ROOM_EDIT_WIDGET_H

#include "gui/widgets/chat-edit-widget.h"

class QCheckBox;
class QLineEdit;

class AccountsComboBox;
class ChatDetailsRoom;

class ChatRoomEditWidget : public ChatEditWidget
{
	Q_OBJECT

	AccountsComboBox *AccountCombo;
	QLineEdit *RoomEdit;
	QLineEdit *NickEdit;
	QLineEdit *PasswordEdit;
	QCheckBox *StayInRoomAfterClosingWindowCheckBox;
	ChatDetailsRoom *RoomDetails;

	void createGui();
	void loadChatData();

private slots:
	void dataChanged();

public:
	explicit ChatRoomEditWidget(const Chat &chat, QWidget *parent = 0);
	virtual ~ChatRoomEditWidget();

public slots:
	virtual void apply();
	virtual void cancel();

};

#endif // CHAT_ROOM_EDIT_WIDGET_H
