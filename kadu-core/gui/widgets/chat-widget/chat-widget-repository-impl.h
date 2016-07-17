/*
 * %kadu copyright begin%
 * Copyright 2013, 2014, 2015 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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
#include "gui/widgets/chat-widget/chat-widget-repository.h"
#include "misc/iterator.h"
#include "exports.h"

#include <QtCore/QObject>
#include <injeqt/injeqt.h>
#include <map>
#include <memory>

class ChatWidget;

/**
 * @addtogroup Gui
 * @{
 */

class KADUAPI ChatWidgetRepositoryImpl : public ChatWidgetRepository
{
	Q_OBJECT

public:
	Q_INVOKABLE explicit ChatWidgetRepositoryImpl(QObject *parent = nullptr);
	virtual ~ChatWidgetRepositoryImpl();

	virtual Iterator begin() override;
	virtual Iterator end() override;

	virtual void addChatWidget(ChatWidget *chatWidget) override;
	virtual bool hasWidgetForChat(const Chat &chat) const override;
	virtual ChatWidget * widgetForChat(const Chat &chat) override;

public slots:
	virtual void removeChatWidget(ChatWidget *chatWidget) override;
	virtual void removeChatWidget(Chat chat) override;

private:
	static ChatWidget * converter(WrappedIterator iterator);

	Storage m_widgets;

};

/**
 * @}
 */
