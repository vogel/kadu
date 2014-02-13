/*
 * %kadu copyright begin%
 * Copyright 2009, 2010, 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009, 2012 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2007, 2009, 2010, 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2010, 2011, 2012, 2013 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2007 Dawid Stawiarski (neeo@kadu.net)
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

#include <QtGui/QTextDocument>

#include "icons/icons-manager.h"
#include "identities/identity.h"
#include "notify/notification-manager.h"
#include "notify/notify-event.h"
#include "parser/parser.h"

#include "connection-error-notification.h"

NotifyEvent *ConnectionErrorNotification::ConnectionErrorNotifyEvent = 0;

static QString getErrorMessage(const ParserData * const object)
{
	const ConnectionErrorNotification * const connectionErrorNotification = dynamic_cast<const ConnectionErrorNotification * const>(object);
	if (connectionErrorNotification)
		return connectionErrorNotification->errorMessage();
	else
		return QString();
}

static QString getErrorServer(const ParserData * const object)
{
	const ConnectionErrorNotification * const connectionErrorNotification = dynamic_cast<const ConnectionErrorNotification * const>(object);
	if (connectionErrorNotification)
		return connectionErrorNotification->errorServer();
	else
		return QString();
}

void ConnectionErrorNotification::registerEvent()
{
	if (ConnectionErrorNotifyEvent)
		return;

	ConnectionErrorNotifyEvent = new NotifyEvent("ConnectionError", NotifyEvent::CallbackNotRequired, QT_TRANSLATE_NOOP("@default", "Connection error"));
	NotificationManager::instance()->registerNotifyEvent(ConnectionErrorNotifyEvent);

	Parser::registerObjectTag("error", getErrorMessage);
	Parser::registerObjectTag("errorServer", getErrorServer);
}

void ConnectionErrorNotification::unregisterEvent()
{
	if (!ConnectionErrorNotifyEvent)
		return;

	Parser::unregisterObjectTag("errorServer");
	Parser::unregisterObjectTag("error");

	NotificationManager::instance()->unregisterNotifyEvent(ConnectionErrorNotifyEvent);
	delete ConnectionErrorNotifyEvent;
	ConnectionErrorNotifyEvent = 0;
}

void ConnectionErrorNotification::notifyConnectionError(const Account &account, const QString &errorServer, const QString &errorMessage)
{
	ConnectionErrorNotification *connectionErrorNotification = new ConnectionErrorNotification(account, errorServer, errorMessage);
	NotificationManager::instance()->notify(connectionErrorNotification);
}

ConnectionErrorNotification::ConnectionErrorNotification(Account account, const QString &errorServer, const QString &errorMessage) :
		AccountNotification(account, "ConnectionError", KaduIcon("dialog-error")),
		ErrorServer(errorServer), ErrorMessage(errorMessage)
{
	setTitle(tr("Connection error"));
	setText(Qt::escape(tr("Connection error on account: %1 (%2)").arg(account.id()).arg(account.accountIdentity().name())));

	if (!ErrorMessage.isEmpty())
	{
		if (ErrorServer.isEmpty())
			setDetails(Qt::escape(ErrorMessage));
		else
			setDetails(Qt::escape(QString("%1 (%2)").arg(ErrorMessage).arg(ErrorServer)));
	}

	addCallback(tr("Ignore"), SLOT(ignoreErrors()), "ignoreErrors()");
}

void ConnectionErrorNotification::ignoreErrors()
{
	NotificationManager::instance()->ignoreConnectionErrors(account());
	emit closed(this);
}

ConnectionErrorNotification::~ConnectionErrorNotification()
{
}

#include "moc_connection-error-notification.cpp"
