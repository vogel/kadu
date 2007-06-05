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

static QString getErrorMessage(const QObject * const object)
{
	return dynamic_cast<const ConnectionErrorNotification * const>(object)->errorMessage();
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

ConnectionErrorNotification::ConnectionErrorNotification(const QString &errorMessage)
	: Notification("ConnectionError", "Blocking", UserListElements()), ErrorMessage(errorMessage)
{
	setTitle(tr("Connection error"));
	setText(tr("<b>Error:</b> %1").arg(ErrorMessage));
}

QString ConnectionErrorNotification::errorMessage() const
{
	kdebugf();

	return ErrorMessage;
}
