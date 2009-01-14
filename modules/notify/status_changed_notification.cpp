/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <QtGui/QApplication>
#include <QtGui/QTextDocument>

#include "accounts/account.h"
#include "contacts/contact-account-data.h"
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

// TODO 0.6.6 what if accountData(account) == null ?
StatusChangedNotification::StatusChangedNotification(const QString &toStatus, ContactList &contacts, Account *account)
	: AccountNotification(QString("StatusChanged/") + toStatus, account->protocol()->statusPixmap(contacts[0].accountData(account)->status()), contacts, account)
{
	const Contact &contact = contacts[0];
	Status status = contact.accountData(account)->status();
	QString syntax;

	if (!status.description().isNull())
		syntax = tr("<b>%1</b> changed status to <i>%2</i><br/> <small>%3</small>");
	else
		syntax = tr("<b>%1</b> changed status to <i>%2</i>");

	setTitle(tr("Status changed"));
	setText(narg(syntax,
		Qt::escape(contact.display()),
		qApp->translate("@default", Status::name(status, false)),
		Qt::escape(status.description())
	));
}
