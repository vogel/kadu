/*
 * %kadu copyright begin%
 * Copyright 2012 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include <QtGui/QFormLayout>
#include <QtGui/QLineEdit>

#include "accounts/filter/protocol-filter.h"
#include "chat/chat-details-room.h"
#include "chat/type/chat-type-room.h"
#include "gui/widgets/accounts-combo-box.h"

#include "chat-room-edit-widget.h"

ChatRoomEditWidget::ChatRoomEditWidget(const Chat &chat, QWidget *parent) :
		ChatEditWidget(chat, parent)
{
	RoomDetails = qobject_cast<ChatDetailsRoom *>(chat.details());

	createGui();
	loadChatData();
}

ChatRoomEditWidget::~ChatRoomEditWidget()
{
}

void ChatRoomEditWidget::createGui()
{
	QFormLayout *layout = new QFormLayout(this);

	AccountCombo = new AccountsComboBox(true, AccountsComboBox::NotVisibleWithOneRowSourceModel, this);
	AccountCombo->setIncludeIdInDisplay(true);

	// only xmpp rooms for now
	// we need to add something like Protocol::supporterChatTypes()
	ProtocolFilter *protocolFilter = new ProtocolFilter(AccountCombo);
	protocolFilter->setProtocolName("jabber");
	AccountCombo->addFilter(protocolFilter);
	connect(AccountCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(dataChanged()));

	layout->addRow(tr("Account:"), AccountCombo);

	RoomEdit = new QLineEdit(this);
	connect(RoomEdit, SIGNAL(textChanged(QString)), this, SLOT(dataChanged()));

	layout->addRow(tr("Room:"), RoomEdit);

	NickEdit = new QLineEdit(this);
	connect(NickEdit, SIGNAL(textChanged(QString)), this, SLOT(dataChanged()));

	layout->addRow(tr("Nick:"), NickEdit);

	PasswordEdit = new QLineEdit(this);
	PasswordEdit->setEchoMode(QLineEdit::Password);
	connect(PasswordEdit, SIGNAL(textChanged(QString)), this, SLOT(dataChanged()));

	layout->addRow(tr("Password:"), PasswordEdit);
}

void ChatRoomEditWidget::dataChanged()
{
	if (!RoomDetails)
		return;

	if (AccountCombo->currentAccount() == chat().chatAccount()
			&& RoomEdit->text() == RoomDetails->room()
			&& NickEdit->text() == RoomDetails->nick()
			&& PasswordEdit->text() == RoomDetails->password())
	{
		setState(StateNotChanged);
		return;
	}

	if (!AccountCombo->currentAccount() || RoomEdit->text().isEmpty() || NickEdit->text().isEmpty())
	{
		setState(StateChangedDataInvalid);
		return;
	}

	Chat sameChat = ChatTypeRoom::findChat(AccountCombo->currentAccount(), RoomEdit->text(), ActionReturnNull);
	if (sameChat && (sameChat != chat()))
	{
		setState(StateChangedDataInvalid);
		return;
	}

	setState(StateChangedDataValid);
}

void ChatRoomEditWidget::loadChatData()
{
	if (!RoomDetails)
		return;

	AccountCombo->setCurrentAccount(chat().chatAccount());
	RoomEdit->setText(RoomDetails->room());
	NickEdit->setText(RoomDetails->nick());
	PasswordEdit->setText(RoomDetails->password());
}

void ChatRoomEditWidget::apply()
{
	if (!RoomDetails)
		return;

	chat().setChatAccount(AccountCombo->currentAccount());
	RoomDetails->setRoom(RoomEdit->text());
	RoomDetails->setNick(NickEdit->text());
	RoomDetails->setPassword(PasswordEdit->text());

	setState(StateNotChanged);
}

void ChatRoomEditWidget::cancel()
{
	loadChatData();

	setState(StateNotChanged);
}
