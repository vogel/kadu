/*
 * %kadu copyright begin%
 * Copyright 2012 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2013 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2013, 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "accounts/account-manager.h"
#include "accounts/account-notification-service.h"
#include "accounts/account.h"
#include "buddies/buddy-manager.h"
#include "chat/chat-manager.h"
#include "chat/chat-storage.h"
#include "multilogon/multilogon-notification-service.h"
#include "multilogon/multilogon-session.h"
#include "notification/notification-configuration.h"
#include "protocols/services/multilogon-service.h"
#include "status/status-notification-service.h"
#include "status/status-type-data.h"
#include "status/status-type-manager.h"
#include "debug.h"

#include "account-event-listener.h"

AccountEventListener::AccountEventListener(QObject *parent) :
			QObject(parent)
{
}

AccountEventListener::~AccountEventListener()
{
}

void AccountEventListener::setAccountManager(AccountManager *accountManager)
{
	m_accountManager = accountManager;
}

void AccountEventListener::setAccountNotificationService(AccountNotificationService *accountNotificationService)
{
	m_accountNotificationService = accountNotificationService;
}

void AccountEventListener::setChatManager(ChatManager *chatManager)
{
	m_chatManager = chatManager;
}

void AccountEventListener::setChatStorage(ChatStorage *chatStorage)
{
	m_chatStorage = chatStorage;
}

void AccountEventListener::setMultilogonNotificationService(MultilogonNotificationService *multilogonNotificationService)
{
	m_multilogonNotificationService = multilogonNotificationService;
}

void AccountEventListener::setNotificationConfiguration(NotificationConfiguration *notificationConfiguration)
{
	m_notificationConfiguration = notificationConfiguration;
}

void AccountEventListener::setStatusNotificationService(StatusNotificationService *statusNotificationService)
{
	m_statusNotificationService = statusNotificationService;
}

void AccountEventListener::setStatusTypeManager(StatusTypeManager *statusTypeManager)
{
	m_statusTypeManager = statusTypeManager;
}

void AccountEventListener::init()
{
	triggerAllAccountsAdded(m_accountManager);
}

void AccountEventListener::done()
{
	triggerAllAccountsRemoved(m_accountManager);
}

void AccountEventListener::accountAdded(Account account)
{
	auto protocol = account.protocolHandler();
	if (!protocol)
		return;

	connect(account, SIGNAL(buddyStatusChanged(Contact, Status)),
			m_statusNotificationService, SLOT(notifyStatusChanged(Contact,Status)));
	connect(account, SIGNAL(connected()), this, SLOT(accountConnected()));
	connect(account, SIGNAL(protocolHandlerChanged(Account)), this, SLOT(protocolHandlerChanged(Account)));
	protocolHandlerChanged(account);
}

void AccountEventListener::accountRemoved(Account account)
{
	disconnect(account, 0, this, 0);
}

void AccountEventListener::protocolHandlerChanged(Account account)
{
	if (account.protocolHandler())
	{
		/* NOTE: We need QueuedConnection here so when the protocol emits the signal, it can cleanup
		* itself before we do something (e.g., reset connection data after invalidPassword, so when
		* we try to log in after entering new password, a new connection can be estabilished instead
		* of giving up because of already existing connection).
		*/
		connect(account.protocolHandler(), SIGNAL(connectionError(Account, const QString &, const QString &)),
				m_accountNotificationService, SLOT(notifyConnectionError(Account,QString,QString)), Qt::QueuedConnection);

		auto multilogonService = account.protocolHandler()->multilogonService();
		if (multilogonService)
		{
			connect(multilogonService, SIGNAL(multilogonSessionConnected(MultilogonSession)),
					m_multilogonNotificationService, SLOT(notifyMultilogonSessionConnected(MultilogonSession)));
			connect(multilogonService, SIGNAL(multilogonSessionDisconnected(MultilogonSession)),
					m_multilogonNotificationService, SLOT(notifyMultilogonSessionDisonnected(MultilogonSession)));
		}
	}
}

void AccountEventListener::accountConnected()
{
	Account account(sender());

	if (!account)
		return;

	if (m_notificationConfiguration->notifyIgnoreOnConnection())
		account.addProperty(QStringLiteral("notify:notify-account-connected"), QDateTime::currentDateTime().addSecs(10), CustomProperties::NonStorable);
}

#include "moc_account-event-listener.cpp"
