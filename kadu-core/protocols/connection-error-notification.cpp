/*
 * %kadu copyright begin%
 * Copyright 2009, 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009, 2012 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2010, 2011, 2012, 2013 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2009, 2010, 2011, 2013, 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "core/core.h"
#include "icons/icons-manager.h"
#include "identities/identity.h"
#include "notification/notification-manager.h"
#include "notification/notification-callback-repository.h"
#include "notification/notification-callback.h"
#include "notification/notification-event.h"
#include "notification/notification-event-repository.h"
#include "parser/parser.h"

#include "connection-error-notification.h"

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
	Core::instance()->notificationEventRepository()->addNotificationEvent(NotificationEvent("ConnectionError", QT_TRANSLATE_NOOP("@default", "Connection error")));

	Parser::registerObjectTag("error", getErrorMessage);
	Parser::registerObjectTag("errorServer", getErrorServer);

	auto connectionIgnoreErrorsDisconnect = NotificationCallback{
		"connection-ignore-errors",
		tr("Ignore"),
		[](Notification *notification){
			auto connectionErrorNotification = qobject_cast<ConnectionErrorNotification *>(notification);
			if (connectionErrorNotification)
				connectionErrorNotification->ignoreErrors();
		}
	};
	Core::instance()->notificationCallbackRepository()->addCallback(connectionIgnoreErrorsDisconnect);
}

void ConnectionErrorNotification::unregisterEvent()
{
	Parser::unregisterObjectTag("errorServer");
	Parser::unregisterObjectTag("error");

	Core::instance()->notificationEventRepository()->removeNotificationEvent(NotificationEvent("ConnectionError", QT_TRANSLATE_NOOP("@default", "Connection error")));
}

void ConnectionErrorNotification::notifyConnectionError(const Account &account, const QString &errorServer, const QString &errorMessage)
{
	ConnectionErrorNotification *connectionErrorNotification = new ConnectionErrorNotification(account, errorServer, errorMessage);
	Core::instance()->notificationManager()->notify(connectionErrorNotification);
}

ConnectionErrorNotification::ConnectionErrorNotification(Account account, const QString &errorServer, const QString &errorMessage) :
		Notification(account, Chat::null, "ConnectionError", KaduIcon("dialog-error")),
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

	addCallback("connection-ignore-errors");
}

void ConnectionErrorNotification::ignoreErrors()
{
	auto account = data()["account"].value<Account>();
	Core::instance()->notificationManager()->ignoreConnectionErrors(account);
	emit closed(this);
}

ConnectionErrorNotification::~ConnectionErrorNotification()
{
}

#include "moc_connection-error-notification.cpp"
