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
#include "accounts/account.h"
#include "buddies/buddy-manager.h"
#include "chat/chat-manager.h"
#include "chat/chat-storage.h"
#include "core/injected-factory.h"
#include "multilogon/multilogon-session.h"
#include "notification/notification/multilogon-notification.h"
#include "notification/notification/status-changed-notification.h"
#include "notification/notification-service.h"
#include "protocols/services/multilogon-service.h"
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

void AccountEventListener::setChatManager(ChatManager *chatManager)
{
	m_chatManager = chatManager;
}

void AccountEventListener::setChatStorage(ChatStorage *chatStorage)
{
	m_chatStorage = chatStorage;
}

void AccountEventListener::setInjectedFactory(InjectedFactory *injectedFactory)
{
	m_injectedFactory = injectedFactory;
}

void AccountEventListener::setNotificationService(NotificationService *notificationService)
{
	m_notificationService = notificationService;
}

void AccountEventListener::setStatusTypeManager(StatusTypeManager *statusTypeManager)
{
	m_statusTypeManager = statusTypeManager;
}

void AccountEventListener::init()
{
	triggerAllAccountsRegistered(m_accountManager);
}

void AccountEventListener::done()
{
	triggerAllAccountsUnregistered(m_accountManager);
}

void AccountEventListener::accountRegistered(Account account)
{
	auto protocol = account.protocolHandler();
	if (!protocol)
		return;

	connect(account, SIGNAL(buddyStatusChanged(Contact, Status)),
			this, SLOT(contactStatusChanged(Contact, Status)));
	connect(account, SIGNAL(connected()), this, SLOT(accountConnected()));

	auto multilogonService = protocol->multilogonService();
	if (multilogonService)
	{
		connect(multilogonService, SIGNAL(multilogonSessionConnected(MultilogonSession*)),
				this, SLOT(multilogonSessionConnected(MultilogonSession*)));
		connect(multilogonService, SIGNAL(multilogonSessionDisconnected(MultilogonSession*)),
				this, SLOT(multilogonSessionDisconnected(MultilogonSession*)));
	}
}

void AccountEventListener::accountUnregistered(Account account)
{
	auto protocol = account.protocolHandler();

	if (!protocol)
		return;

	disconnect(account, 0, this, 0);

	auto multilogonService = protocol->multilogonService();
	if (multilogonService)
		disconnect(multilogonService, 0, this, 0);
}

void AccountEventListener::accountConnected()
{
	Account account(sender());

	if (!account)
		return;

	if (m_notificationService->notifyIgnoreOnConnection())
		account.addProperty("notify:notify-account-connected", QDateTime::currentDateTime().addSecs(10), CustomProperties::NonStorable);
}

void AccountEventListener::contactStatusChanged(Contact contact, Status oldStatus)
{
	if (contact.isAnonymous() || !contact.contactAccount())
		return;

	auto protocol = contact.contactAccount().protocolHandler();
	if (!protocol || !protocol->isConnected())
		return;

	if (m_notificationService->notifyIgnoreOnConnection())
	{
		QDateTime dateTime = contact.contactAccount().property("notify:notify-account-connected", QDateTime()).toDateTime();
		if (dateTime.isValid() && dateTime >= QDateTime::currentDateTime())
			return;
	}

	if (!contact.ownerBuddy().property("notify:Notify", true).toBool())
	{
		kdebugmf(KDEBUG_FUNCTION_END, "end: not notifying user AND not notifying all users\n");
		return;
	}

	if (contact == contact.contactAccount().accountContact()) // myself
		return;

	auto status = contact.currentStatus();
	if (oldStatus == status)
		return;

	auto statusDisplayName = status.displayName();
	auto description = status.description();

	if (contact.ownerBuddy().property("kadu:HideDescription", false).toBool())
	{
		if (oldStatus.type() == status.type())
			return;
		else
			description.clear();
	}

	if (m_notificationService->ignoreOnlineToOnline() &&
			!status.isDisconnected() &&
			!oldStatus.isDisconnected())
		return;

	auto const &typeData = m_statusTypeManager->statusTypeData(status.type());
	QString changedTo = "/To" + typeData.name();

	auto statusChangedNotification = m_injectedFactory->makeInjected<StatusChangedNotification>(m_statusTypeManager, m_chatManager, m_chatStorage, changedTo, contact, statusDisplayName, description);
	m_notificationService->notify(statusChangedNotification);
}

void AccountEventListener::multilogonSessionConnected(MultilogonSession *session)
{
	MultilogonNotification *notification = m_injectedFactory->makeInjected<MultilogonNotification>(session, "multilogon/sessionConnected", true);
	notification->setTitle(tr("Multilogon"));
	notification->setText(tr("Multilogon session connected from %1 at %2 with %3 for %4 account")
			.arg(session->remoteAddress().toString())
			.arg(session->logonTime().toString())
			.arg(session->name())
			.arg(session->account().id()));

	m_notificationService->notify(notification);
}

void AccountEventListener::multilogonSessionDisconnected(MultilogonSession *session)
{
	MultilogonNotification *notification = m_injectedFactory->makeInjected<MultilogonNotification>(session, "multilogon/sessionDisconnected", false);
	notification->setTitle(tr("Multilogon"));
	notification->setText(tr("Multilogon session disconnected from %1 at %2 with %3 for %4 account")
			.arg(session->remoteAddress().toString())
			.arg(session->logonTime().toString())
			.arg(session->name())
			.arg(session->account().id()));

	m_notificationService->notify(notification);
}

#include "moc_account-event-listener.cpp"
