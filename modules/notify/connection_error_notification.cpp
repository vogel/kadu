/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "debug.h"
#include "kadu_parser.h"

#include "connection_error_notification.h"

QStringList ConnectionErrorNotification::ActiveErrors;

static QString getErrorMessage(const QObject * const object)
{
	const ConnectionErrorNotification * const connectionErrorNotification = dynamic_cast<const ConnectionErrorNotification * const>(object);
	if (connectionErrorNotification)
		return connectionErrorNotification->errorMessage();
	else
		return "";
}

void ConnectionErrorNotification::registerEvent(Notify *manager)
{
	manager->registerEvent("ConnectionError", "Connection error", CallbackNotRequired);
	KaduParser::registerObjectTag("error", getErrorMessage);
}

void ConnectionErrorNotification::unregisterEvent(Notify *manager)
{
	KaduParser::unregisterObjectTag("error", getErrorMessage);
	manager->unregisterEvent("ConnectionError");
}

bool ConnectionErrorNotification::activeError(const QString &errorMessage)
{
	return ActiveErrors.find(errorMessage) != ActiveErrors.end();
}

ConnectionErrorNotification::ConnectionErrorNotification(const QString &errorMessage)
	: Notification("ConnectionError", "Blocking", UserListElements()), ErrorMessage(errorMessage)
{
	setTitle(tr("Connection error"));
	setText(tr("<b>Error:</b> %1").arg(ErrorMessage));

	ActiveErrors.append(ErrorMessage);
}

ConnectionErrorNotification::~ConnectionErrorNotification()
{
	ActiveErrors.remove(ErrorMessage);
}

QString ConnectionErrorNotification::errorMessage() const
{
	kdebugf();

	return ErrorMessage;
}
