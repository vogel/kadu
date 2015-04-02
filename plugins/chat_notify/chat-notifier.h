/*
 * %kadu copyright begin%
 * Copyright 2014 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2011, 2012, 2013, 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include <QtCore/QPointer>

#include "notification/notifier.h"

class ChatWidget;
class ChatWidgetRepository;
class FormattedStringFactory;

class ChatNotifier : public Notifier
{
	Q_OBJECT

public:
	explicit ChatNotifier(QObject *parent = 0);
	virtual ~ChatNotifier();

	void setChatWidgetRepository(ChatWidgetRepository *chatWidgetRepository);
	void setFormattedStringFactory(FormattedStringFactory *formattedStringFactory);

	virtual NotifierConfigurationWidget * createConfigurationWidget(QWidget *parent = 0);

	virtual void notify(Notification *notification);

private:
	QPointer<ChatWidgetRepository> m_chatWidgetRepository;
	QPointer<FormattedStringFactory> m_formattedStringFactory;

	void sendNotificationToChatWidget(Notification *notification, ChatWidget *chatWidget);

};
