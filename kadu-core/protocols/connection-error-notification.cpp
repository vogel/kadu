/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "notify/notification-manager.h"
#include "notify/notify-event.h"
#include "debug.h"
#include "icons_manager.h"
#include "kadu_parser.h"

#include "connection-error-notification.h"

NotifyEvent *ConnectionErrorNotification::ConnectionErrorNotifyEvent = 0;
QMap<Account *, QStringList> ConnectionErrorNotification::ActiveErrors;

static QString getErrorMessage(const QObject * const object)
{
	const ConnectionErrorNotification * const connectionErrorNotification = dynamic_cast<const ConnectionErrorNotification * const>(object);
	if (connectionErrorNotification)
		return connectionErrorNotification->errorMessage();
	else
		return "";
}

static QString getErrorServer(const QObject * const object)
{
	const ConnectionErrorNotification * const connectionErrorNotification = dynamic_cast<const ConnectionErrorNotification * const>(object);
	if (connectionErrorNotification)
		return connectionErrorNotification->errorServer();
	else
		return "";
}

void ConnectionErrorNotification::registerEvent()
{
	if (ConnectionErrorNotifyEvent)
		return;

	ConnectionErrorNotifyEvent = new NotifyEvent("ConnectionError", NotifyEvent::CallbackNotRequired, QT_TRANSLATE_NOOP("@default", "Connection error"));
	NotificationManager::instance()->registerNotifyEvent(ConnectionErrorNotifyEvent);
	KaduParser::registerObjectTag("error", getErrorMessage);
	KaduParser::registerObjectTag("errorServer", getErrorServer);
}

void ConnectionErrorNotification::unregisterEvent()
{
	KaduParser::registerObjectTag("errorServer", getErrorServer);
	KaduParser::unregisterObjectTag("error", getErrorMessage);

	NotificationManager::instance()->unregisterNotifyEvent(ConnectionErrorNotifyEvent);
	delete ConnectionErrorNotifyEvent;
	ConnectionErrorNotifyEvent = 0;
}

bool ConnectionErrorNotification::activeError(Account *account, const QString &errorMessage)
{
	return ActiveErrors.contains(account) && ActiveErrors[account].contains(errorMessage);
}

ConnectionErrorNotification::ConnectionErrorNotification(Account *account, const QString &errorServer, const QString &errorMessage)
	: AccountNotification(account, "ConnectionError", icons_manager->loadIcon("CriticalSmall"), ContactList()), ErrorServer(errorServer), ErrorMessage(errorMessage)
{
	setTitle(tr("Connection error"));
	setText(tr("<b>Error:</b> (%1) %2").arg(ErrorServer).arg(ErrorMessage));

	ActiveErrors[account].append(ErrorMessage);
}

ConnectionErrorNotification::~ConnectionErrorNotification()
{
	ActiveErrors[account()].removeAll(ErrorMessage);
}

QString ConnectionErrorNotification::errorMessage() const
{
	kdebugf();

	return ErrorMessage;
}

QString ConnectionErrorNotification::errorServer() const
{
	kdebugf();

	return ErrorServer;
}
