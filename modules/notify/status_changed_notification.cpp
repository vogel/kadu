/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <QApplication>
#include <QTextDocument>

#include "misc.h"

#include "status_changed_notification.h"

void StatusChangedNotification::registerEvents(Notify *manager)
{
	manager->registerEvent("StatusChanged/ToOnline", QT_TRANSLATE_NOOP("@default", "User changed status to online"), CallbackNotRequired);
	manager->registerEvent("StatusChanged/ToBusy", QT_TRANSLATE_NOOP("@default", "User changed status to busy"), CallbackNotRequired);
	manager->registerEvent("StatusChanged/ToInvisible", QT_TRANSLATE_NOOP("@default", "User changed status to invisible"), CallbackNotRequired);
	manager->registerEvent("StatusChanged/ToOffline", QT_TRANSLATE_NOOP("@default", "User changed status to offline"), CallbackNotRequired);
}

void StatusChangedNotification::unregisterEvents(Notify *manager)
{
	manager->unregisterEvent("StatusChanged/ToOnline");
	manager->unregisterEvent("StatusChanged/ToBusy");
	manager->unregisterEvent("StatusChanged/ToInvisible");
	manager->unregisterEvent("StatusChanged/ToOffline");
}

StatusChangedNotification::StatusChangedNotification(const QString &toStatus, const UserListElements &userListElements, const QString &protocolName)
	: ProtocolNotification(QString("StatusChanged/") + toStatus, userListElements[0].status(protocolName).pixmapName(), userListElements, protocolName)
{
	const UserListElement &ule = userListElements[0];
	QString syntax;

	if (ule.status(protocolName).hasDescription())
		syntax = tr("<b>%1</b> changed status to <i>%2</i><br/> <small>%3</small>");
	else
		syntax = tr("<b>%1</b> changed status to <i>%2</i>");

	setTitle(tr("Status changed"));
	setText(narg(syntax,
		Qt::escape(ule.altNick()),
		qApp->translate("@default", ule.status(protocolName).name().ascii()),
		Qt::escape(ule.status(protocolName).description())
	));
}
