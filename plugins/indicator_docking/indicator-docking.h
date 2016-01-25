/*
 * %kadu copyright begin%
 * Copyright 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include <QtCore/QObject>
#include <QtCore/QPointer>
#include <injeqt/injeqt.h>

class ChatManager;
class ChatWidgetManager;
class Message;
class QMessagingMenuApp;
class StatusContainerManager;
class StatusContainer;
class StatusTypeManager;
class UnreadMessageRepository;
enum class QMessagingMenuStatus;

class IndicatorDocking : public QObject
{
	Q_OBJECT

public:
	Q_INVOKABLE explicit IndicatorDocking(QObject *parent = nullptr);
	virtual ~IndicatorDocking();

private:
	QPointer<ChatManager> m_chatManager;
	QPointer<ChatWidgetManager> m_chatWidgetManager;
	QPointer<StatusContainerManager> m_statusContainerManager;
	QPointer<StatusTypeManager> m_statusTypeManager;
	QPointer<UnreadMessageRepository> m_unreadMessageRepository;

	QMessagingMenuApp *m_messagingMenuApp;

private slots:
	INJEQT_SET void setChatManager(ChatManager *chatManager);
	INJEQT_SET void setChatWidgetManager(ChatWidgetManager *chatWidgetManager);
	INJEQT_SET void setStatusContainerManager(StatusContainerManager *statusContainerManager);
	INJEQT_SET void setStatusTypeManager(StatusTypeManager *statusTypeManager);
	INJEQT_SET void setUnreadMessageRepository(UnreadMessageRepository *unreadMessageRepository);

	void unreadMessageAdded(const Message &message);
	void unreadMessageRemoved(const Message &message);

	void sourceActivated(const QString &id);
	void statusChanged(QMessagingMenuStatus status);
	void statusContainerUpdated(StatusContainer *statusContainer);

};
