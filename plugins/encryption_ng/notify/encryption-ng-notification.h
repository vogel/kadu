/*
 * %kadu copyright begin%
 * Copyright 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2011 Tomasz Rostanski (rozteck@interia.pl)
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

#ifndef ENCRYPTION_NG_NOTIFICATION_H
#define ENCRYPTION_NG_NOTIFICATION_H

#include "notify/notification.h"

#include "../encryption_exports.h"

class Contact;
class NotifyEvent;

class ENCRYPTIONAPI EncryptionNgNotification : public Notification
{
	Q_OBJECT

	static NotifyEvent *EncryptionNotification;
	static NotifyEvent *PublicKeySentNotification;
	static NotifyEvent *PublicKeySendErrorNotification;
	static NotifyEvent *EncryptionErrorNotification;

public:
	static void registerNotifications();
	static void unregisterNotifications();

	static void notifyPublicKeySent(Contact contact);
	static void notifyPublicKeySendError(Contact contact, const QString &error);
	static void notifyEncryptionError(const QString &error);

	explicit EncryptionNgNotification(const QString &name);
	virtual ~EncryptionNgNotification();

};


#endif // ENCRYPTION_NG_NOTIFICATION_H
