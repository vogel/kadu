/*
 * %kadu copyright begin%
 * Copyright 2012 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2011, 2012, 2013 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2011, 2013, 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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
#include "notification/notification-manager.h"
#include "notification/notification-event.h"
#include "notification/notification-event-repository.h"

#include "encryption-ng-notification.h"

void EncryptionNgNotification::registerNotifications()
{
	Core::instance()->notificationEventRepository()->addNotificationEvent(NotificationEvent("encryption-ng", QT_TRANSLATE_NOOP("@default", "Encryption")));
	Core::instance()->notificationEventRepository()->addNotificationEvent(NotificationEvent("encryption-ng/publicKeySent", QT_TRANSLATE_NOOP("@default", "Public key has been sent")));
	Core::instance()->notificationEventRepository()->addNotificationEvent(NotificationEvent("encryption-ng/publicKeySendError", QT_TRANSLATE_NOOP("@default", "Error during sending public key")));
	Core::instance()->notificationEventRepository()->addNotificationEvent(NotificationEvent("encryption-ng/encryptionError", QT_TRANSLATE_NOOP("@default", "Encryption error has occured")));
}

void EncryptionNgNotification::unregisterNotifications()
{
	Core::instance()->notificationEventRepository()->removeNotificationEvent(NotificationEvent("encryption-ng", QT_TRANSLATE_NOOP("@default", "Encryption")));
	Core::instance()->notificationEventRepository()->removeNotificationEvent(NotificationEvent("encryption-ng/publicKeySent", QT_TRANSLATE_NOOP("@default", "Public key has been sent")));
	Core::instance()->notificationEventRepository()->removeNotificationEvent(NotificationEvent("encryption-ng/publicKeySendError", QT_TRANSLATE_NOOP("@default", "Error during sending public key")));
	Core::instance()->notificationEventRepository()->removeNotificationEvent(NotificationEvent("encryption-ng/encryptionError", QT_TRANSLATE_NOOP("@default", "Encryption error has occured")));
}

void EncryptionNgNotification::notifyPublicKeySent(Contact contact)
{
	EncryptionNgNotification *notification = new EncryptionNgNotification("encryption-ng/publicKeySent");
	notification->setTitle(tr("Encryption"));
	notification->setText(Qt::escape(tr("Public key has been send to: %1 (%2)").arg(contact.display(true)).arg(contact.id())));
	Core::instance()->notificationManager()->notify(notification);
}

void EncryptionNgNotification::notifyPublicKeySendError(Contact contact, const QString &error)
{
	EncryptionNgNotification *notification = new EncryptionNgNotification("encryption-ng/publicKeySendError");
	notification->setTitle(tr("Encryption"));
	notification->setText(Qt::escape(tr("Error sending public key to: %1 (%2)").arg(contact.display(true)).arg(contact.id())));
	notification->setDetails(Qt::escape(error));
	Core::instance()->notificationManager()->notify(notification);
}

void EncryptionNgNotification::notifyEncryptionError(const QString &error)
{
	EncryptionNgNotification *notification = new EncryptionNgNotification("encryption-ng/encryptionError");
	notification->setTitle(tr("Encryption"));
	notification->setText(tr("Error occured during encryption"));
	notification->setDetails(Qt::escape(error));
	Core::instance()->notificationManager()->notify(notification);
}

EncryptionNgNotification::EncryptionNgNotification(const QString &name) :
		Notification(Account::null, Chat::null, name, KaduIcon("security-high")), Name(name)
{
}

EncryptionNgNotification::~EncryptionNgNotification()
{

}

#include "moc_encryption-ng-notification.cpp"
