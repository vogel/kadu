/*
 * %kadu copyright begin%
 * Copyright 2007 Dawid Stawiarski (neeo@kadu.net)
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2007, 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2009 Piotr Galiszewski (piotrgaliszewski@gmail.com)
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

#include "gui/windows/password-window.h"
#include "notify/notification-manager.h"
#include "notify/notify-event.h"
#include "parser/parser.h"
#include "protocols/protocol.h"
#include "debug.h"
#include "icons-manager.h"

#include "invalid-password-notification.h"

NotifyEvent *InvalidPasswordNotification::InvalidPasswordNotifyEvent = 0;
QSet<Account> InvalidPasswordNotification::ActiveErrors;

void InvalidPasswordNotification::registerEvent()
{
	if (InvalidPasswordNotifyEvent)
		return;

	InvalidPasswordNotifyEvent = new NotifyEvent("InvalidPassword", NotifyEvent::CallbackRequired, QT_TRANSLATE_NOOP("@default", "Invalid password"));
	NotificationManager::instance()->registerNotifyEvent(InvalidPasswordNotifyEvent);
}

void InvalidPasswordNotification::unregisterEvent()
{
	if (!InvalidPasswordNotifyEvent)
		return;

	NotificationManager::instance()->unregisterNotifyEvent(InvalidPasswordNotifyEvent);
	delete InvalidPasswordNotifyEvent;
	InvalidPasswordNotifyEvent = 0;
}

bool InvalidPasswordNotification::activeError(Account account)
{
	return ActiveErrors.contains(account);
}

InvalidPasswordNotification::InvalidPasswordNotification(Account account) :
		AccountNotification(account, "InvalidPassword", "dialog-error")
{
	setTitle(tr("Invalid password"));
	setText(tr("<b>Invalid password for:</b> %1 (%2)").arg(account.accountIdentity().name()).arg(account.id()));
	setDefaultCallback(30000, SLOT(callbackDiscard()));

	addCallback(tr("Enter new password"), SLOT(callbackAccept()), "callbackAccept()");
	addCallback(tr("Ignore"), SLOT(callbackDiscard()), "callbackDiscard()");

	ActiveErrors.insert(account);
}

InvalidPasswordNotification::~InvalidPasswordNotification()
{
	ActiveErrors.remove(account());
}

bool InvalidPasswordNotification::requireCallback()
{
	return true;
}

bool InvalidPasswordNotification::ignoreSilentMode()
{
	return true;
}

void InvalidPasswordNotification::callbackDiscard()
{
  	close();

	if (!account().protocolHandler())
		return;
	
	account().protocolHandler()->login(QString(), false);
}

void InvalidPasswordNotification::callbackAccept()
{
	close();

	if (!account().protocolHandler())
		return;

	QString message = tr("Please provide valid password for %1 (%2) account")
			.arg(account().accountIdentity().name())
			.arg(account().id());
	PasswordWindow::getPassword(message, account().protocolHandler(), SLOT(login(const QString &, bool)));
}
