/*
 * %kadu copyright begin%
 * Copyright 2014 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2013 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "chat/chat.h"

#include <QtCore/QPointer>
#include <QtWidgets/QWidget>
#include <injeqt/injeqt.h>

class QVBoxLayout;

class ChatTopBarWidgetFactory;
class ChatTopBarWidgetFactoryRepository;

class ChatTopBarContainerWidget : public QWidget
{
	Q_OBJECT

	QPointer<ChatTopBarWidgetFactoryRepository> m_chatTopBarWidgetFactoryRepository;

	Chat MyChat;
	QVBoxLayout *Layout;
	QMap<ChatTopBarWidgetFactory *, QWidget *> TopBarWidgets;

	void createGui();

private slots:
	INJEQT_SET void setChatTopBarWidgetFactoryRepository(ChatTopBarWidgetFactoryRepository *chatTopBarWidgetFactoryRepository);
	INJEQT_INIT void init();

	void factoryRegistered(ChatTopBarWidgetFactory *factory);
	void factoryUnregistered(ChatTopBarWidgetFactory *factory);

public:
	explicit ChatTopBarContainerWidget(const Chat &chat, QWidget *parent = nullptr);
	virtual ~ChatTopBarContainerWidget();

};
