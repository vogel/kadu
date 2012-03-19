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

#include "chat-room-edit-widget.h"

ChatRoomEditWidget::ChatRoomEditWidget(const Chat &chat, QWidget *parent) :
		ChatEditWidget(chat, parent)
{
	createGui();
}

ChatRoomEditWidget::~ChatRoomEditWidget()
{
}

void ChatRoomEditWidget::createGui()
{
	QFormLayout *layout = new QFormLayout(this);

	RoomEdit = new QLineEdit(this);
	// connect(RoomEdit, SIGNAL(textChanged(QString)), this, SLOT(validateData()));

	layout->addRow(tr("Room:"), RoomEdit);

	PasswordEdit = new QLineEdit(this);
	PasswordEdit->setEchoMode(QLineEdit::Password);
	// connect(PasswordEdit, SIGNAL(textChanged(QString)), this, SLOT(validateData()));

	layout->addRow(tr("Password:"), PasswordEdit);
}

void ChatRoomEditWidget::apply()
{
}

void ChatRoomEditWidget::cancel()
{
}
