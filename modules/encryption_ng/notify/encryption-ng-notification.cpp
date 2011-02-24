/*
 * %kadu copyright begin%
 * Copyright 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "notify/notification-manager.h"
#include "notify/notify-event.h"
#include "icons-manager.h"

#include "encryption-ng-notification.h"

NotifyEvent * EncryptionNgNotification::EncryptionNotification = 0;
NotifyEvent * EncryptionNgNotification::PublicKeySentNotification = 0;
NotifyEvent * EncryptionNgNotification::PublicKeySendErrorNotification = 0;
NotifyEvent * EncryptionNgNotification::EncryptionErrorNotification = 0;

void EncryptionNgNotification::registerNotifications()
{
	if (!EncryptionNotification)
	{
		EncryptionNotification = new NotifyEvent("encryption-ng", NotifyEvent::CallbackNotRequired, "Encryption");
		NotificationManager::instance()->registerNotifyEvent(EncryptionNotification);
	}

	if (!PublicKeySentNotification)
	{
		PublicKeySentNotification = new NotifyEvent("encryption-ng/publicKeySent", NotifyEvent::CallbackNotRequired, "Public key has been sent");
		NotificationManager::instance()->registerNotifyEvent(PublicKeySentNotification);
	}

	if (!PublicKeySendErrorNotification)
	{
		PublicKeySendErrorNotification = new NotifyEvent("encryption-ng/publicKeySendError", NotifyEvent::CallbackNotRequired, "Error during sending public key");
		NotificationManager::instance()->registerNotifyEvent(PublicKeySendErrorNotification);
	}

	if (!EncryptionErrorNotification)
	{
		EncryptionErrorNotification = new NotifyEvent("encryption-ng/encryptionError", NotifyEvent::CallbackNotRequired, "Encryption error has occured");
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
	notification->setText(tr("Public key has been send to: %1 (%2)").arg(contact.ownerBuddy().display()).arg(contact.id()));
	NotificationManager::instance()->notify(notification);
}

void EncryptionNgNotification::notifyPublicKeySendError(Contact contact, const QString &error)
{
	EncryptionNgNotification *notification = new EncryptionNgNotification("encryption-ng/publicKeySendError");
	notification->setTitle(tr("Encryption"));
	notification->setText(tr("Error \"%3\" during sending public key to: %1 (%2)").arg(contact.ownerBuddy().display()).arg(contact.id()).arg(error));
	NotificationManager::instance()->notify(notification);
}

void EncryptionNgNotification::notifyEncryptionError(const QString &error)
{
	EncryptionNgNotification *notification = new EncryptionNgNotification("encryption-ng/encryptionError");
	notification->setTitle(tr("Encryption"));
	notification->setText(tr("Error occured during encryption: %1").arg(error));
	NotificationManager::instance()->notify(notification);
}

EncryptionNgNotification::EncryptionNgNotification(const QString &name) :
		Notification(name, "security-high")
{
}

EncryptionNgNotification::~EncryptionNgNotification()
{

}
