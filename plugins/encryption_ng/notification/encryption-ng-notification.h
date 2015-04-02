/*
 * %kadu copyright begin%
 * Copyright 2012 Wojciech Treter (juzefwt@gmail.com)
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

#ifndef ENCRYPTION_NG_NOTIFICATION_H
#define ENCRYPTION_NG_NOTIFICATION_H

#include "notification/notification/notification.h"

#include "../encryption_exports.h"

class Contact;

class ENCRYPTIONAPI EncryptionNgNotification : public Notification
{
	Q_OBJECT

	QString Name;

public:
	static void registerNotifications();
	static void unregisterNotifications();

	static void notifyPublicKeySent(Contact contact);
	static void notifyPublicKeySendError(Contact contact, const QString &error);
	static void notifyEncryptionError(const QString &error);

	virtual QString groupKey() const { return Name; }

	explicit EncryptionNgNotification(const QString &name);
	virtual ~EncryptionNgNotification();

};


#endif // ENCRYPTION_NG_NOTIFICATION_H
