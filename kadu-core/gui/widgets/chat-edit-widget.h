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

#ifndef CHAT_EDIT_WIDGET_H
#define CHAT_EDIT_WIDGET_H

#include <QtGui/QWidget>

#include "chat/chat.h"
#include "gui/widgets/modal-configuration-widget.h"
#include "exports.h"

class KADUAPI ChatEditWidget : public ModalConfigurationWidget
{
	Q_OBJECT

	Chat MyChat;

protected:
	Chat chat() { return MyChat; }

public:
	explicit ChatEditWidget(Chat chat, QWidget *parent = 0) :
			ModalConfigurationWidget(parent), MyChat(chat) {}
	virtual ~ChatEditWidget() {}

};

#endif // CHAT_EDIT_WIDGET_H
