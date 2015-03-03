/*
 * %kadu copyright begin%
 * Copyright 2011, 2012, 2014 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#include "protocols/services/chat-state-service.h"

#include <QtCore/QHash>
#include <QtCore/QPointer>

class JabberResourceService;
class Jid;

class QXmppClient;
class QXmppMessage;

class JabberChatStateService : public ChatStateService
{
	Q_OBJECT

public:
	explicit JabberChatStateService(QXmppClient *client, Account account, QObject *parent = nullptr);
	virtual ~JabberChatStateService();

	void setResourceService(JabberResourceService *resourceService);

	virtual void sendState(const Contact &contact, ChatState state) override;

	void extractReceivedChatState(const QXmppMessage &message);
	QXmppMessage withSentChatState(QXmppMessage message);

private:
	QPointer<QXmppClient> m_client;
	QPointer<JabberResourceService> m_resourceService;

};
