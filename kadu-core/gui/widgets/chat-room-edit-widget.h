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

#pragma once

#include "gui/widgets/chat-edit-widget.h"

#include <QtCore/QPointer>
#include <injeqt/injeqt.h>

class AccountsComboBox;
class ChatDetailsRoom;
class ChatManager;
class InjectedFactory;

class QCheckBox;
class QLineEdit;

class ChatRoomEditWidget : public ChatEditWidget
{
	Q_OBJECT

public:
	explicit ChatRoomEditWidget(const Chat &chat, QWidget *parent = nullptr);
	virtual ~ChatRoomEditWidget();

public slots:
	virtual void apply();
	virtual void cancel();

private:
	QPointer<ChatManager> m_chatManager;
	QPointer<InjectedFactory> m_injectedFactory;

	AccountsComboBox *AccountCombo;
	QLineEdit *RoomEdit;
	QLineEdit *NickEdit;
	QLineEdit *PasswordEdit;
	QCheckBox *StayInRoomAfterClosingWindowCheckBox;
	ChatDetailsRoom *RoomDetails;

	void createGui();
	void loadChatData();

private slots:
	INJEQT_SET void setChatManager(ChatManager *chatManager);
	INJEQT_SET void setInjectedFactory(InjectedFactory *injectedFactory);
	INJEQT_INIT void init();

	void dataChanged();

};
