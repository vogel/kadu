/*
 * %kadu copyright begin%
 * Copyright 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2012 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2011, 2013 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2011, 2012, 2013 Bartosz Brachaczek (b.brachaczek@gmail.com)
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
#include "notify/notification-manager.h"
#include "notify/notify-event.h"

#include "encryption-ng-notification.h"

NotifyEvent * EncryptionNgNotification::EncryptionNotification = 0;
NotifyEvent * EncryptionNgNotification::PublicKeySentNotification = 0;
NotifyEvent * EncryptionNgNotification::PublicKeySendErrorNotification = 0;
NotifyEvent * EncryptionNgNotification::EncryptionErrorNotification = 0;

void EncryptionNgNotification::registerNotifications()
{
	if (!EncryptionNotification)
	{
		EncryptionNotification = new NotifyEvent("encryption-ng", NotifyEvent::CallbackNotRequired, QT_TRANSLATE_NOOP("@default", "Encryption"));
		NotificationManager::instance()->registerNotifyEvent(EncryptionNotification);
	}

	if (!PublicKeySentNotification)
	{
		PublicKeySentNotification = new NotifyEvent("encryption-ng/publicKeySent", NotifyEvent::CallbackNotRequired, QT_TRANSLATE_NOOP("@default", "Public key has been sent"));
		NotificationManager::instance()->registerNotifyEvent(PublicKeySentNotification);
	}

	if (!PublicKeySendErrorNotification)
	{
		PublicKeySendErrorNotification = new NotifyEvent("encryption-ng/publicKeySendError", NotifyEvent::CallbackNotRequired, QT_TRANSLATE_NOOP("@default", "Error during sending public key"));
		NotificationManager::instance()->registerNotifyEvent(PublicKeySendErrorNotification);
	}

	if (!EncryptionErrorNotification)
	{
		EncryptionErrorNotification = new NotifyEvent("encryption-ng/encryptionError", NotifyEvent::CallbackNotRequired, QT_TRANSLATE_NOOP("@default", "Encryption error has occured"));
		NotificationManager::instance()->registerNotifyEvent(EncryptionErrorNotification);
	}
}

void EncryptionNgNotification::unregisterNotifications()
{
	if (EncryptionNotification)
	{
		NotificationManager::instance()->unregisterNotifyEvent(EncryptionNotification);
		delete EncryptionNotification;
		EncryptionNotification = 0;
	}

	if (PublicKeySentNotification)
	{
		NotificationManager::instance()->unregisterNotifyEvent(PublicKeySentNotification);
		delete PublicKeySentNotification;
		PublicKeySentNotification = 0;
	}

	if (PublicKeySendErrorNotification)
	{
		NotificationManager::instance()->unregisterNotifyEvent(PublicKeySendErrorNotification);
		delete PublicKeySendErrorNotification;
		PublicKeySendErrorNotification = 0;
	}

	if (EncryptionErrorNotification)
	{
		NotificationManager::instance()->unregisterNotifyEvent(EncryptionErrorNotification);
		delete EncryptionErrorNotification;
		EncryptionErrorNotification = 0;
	}
}

void EncryptionNgNotification::notifyPublicKeySent(Contact contact)
{
	EncryptionNgNotification *notification = new EncryptionNgNotification("encryption-ng/publicKeySent");
	notification->setTitle(tr("Encryption"));
	notification->setText(Qt::escape(tr("Public key has been send to: %1 (%2)").arg(contact.display(true)).arg(contact.id())));
	NotificationManager::instance()->notify(notification);
}

void EncryptionNgNotification::notifyPublicKeySendError(Contact contact, const QString &error)
{
	EncryptionNgNotification *notification = new EncryptionNgNotification("encryption-ng/publicKeySendError");
	notification->setTitle(tr("Encryption"));
	notification->setText(Qt::escape(tr("Error sending public key to: %1 (%2)").arg(contact.display(true)).arg(contact.id())));
	notification->setDetails(Qt::escape(error));
	NotificationManager::instance()->notify(notification);
}

void EncryptionNgNotification::notifyEncryptionError(const QString &error)
{
	EncryptionNgNotification *notification = new EncryptionNgNotification("encryption-ng/encryptionError");
	notification->setTitle(tr("Encryption"));
	notification->setText(tr("Error occured during encryption"));
	notification->setDetails(Qt::escape(error));
	NotificationManager::instance()->notify(notification);
}

EncryptionNgNotification::EncryptionNgNotification(const QString &name) :
		Notification(name, KaduIcon("security-high")), Name(name)
{
}

EncryptionNgNotification::~EncryptionNgNotification()
{

}

#include "moc_encryption-ng-notification.cpp"
