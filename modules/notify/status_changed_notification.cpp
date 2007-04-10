/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <qapplication.h>
#include <qstylesheet.h>

#include "misc.h"

#include "status_changed_notification.h"

void StatusChangedNotification::registerEvents(Notify *manager)
{
	manager->registerEvent("StatusChanged/ToOnline", "User changed status to online", CallbackNotRequired);
	manager->registerEvent("StatusChanged/ToBusy", "User changed status to busy", CallbackNotRequired);
	manager->registerEvent("StatusChanged/ToInvisible", "User changed status to invisible", CallbackNotRequired);
	manager->registerEvent("StatusChanged/ToOffline", "User changed status to offline", CallbackNotRequired);
}

void StatusChangedNotification::unregisterEvents(Notify *manager)
{
	manager->unregisterEvent("StatusChanged/ToOnline");
	manager->unregisterEvent("StatusChanged/ToBusy");
	manager->unregisterEvent("StatusChanged/ToInvisible");
	manager->unregisterEvent("StatusChanged/ToOffline");
}

StatusChangedNotification::StatusChangedNotification(const QString &toStatus, const UserListElements &userListElements)
	: Notification(QString("StatusChanged/") + toStatus, userListElements[0].status("Gadu").pixmapName(), userListElements)
{
	const UserListElement &ule = userListElements[0];
	QString syntax;

	if (ule.status("Gadu").hasDescription())
		syntax = "<b>%1</b> changed status to <i>%2</i><br/> <small>%3</small>";
	else
		syntax = "<b>%1</b> changed status to <i>%2</i>";

	setTitle(tr("Status changed"));
	setText(narg(tr(syntax),
		ule.altNick(),
		qApp->translate("@default", ule.status("Gadu").name().ascii()),
		QStyleSheet::escape(ule.status("Gadu").description())
	));
}
