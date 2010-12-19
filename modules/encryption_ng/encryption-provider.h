/*
 * %kadu copyright begin%
 * Copyright 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#ifndef ENCRYPTION_PROVIDER_H
#define ENCRYPTION_PROVIDER_H

#include <QtCore/QObject>

#include "chat/chat.h"

class Decryptor;
class Encryptor;

class EncryptionProvider : public QObject
{
	Q_OBJECT

public:
	virtual ~EncryptionProvider() {}

	virtual bool canEncrypt(const Chat &chat) = 0;
	virtual bool canDecrypt(const Chat &chat) = 0;

	virtual Encryptor * encryptor(const Chat &chat) = 0;
	virtual Decryptor * decryptor(const Chat &chat) = 0;

signals:
	void canEncryptChanged(const Chat &chat);
	void canDecryptChanged(const Chat &chat);
	void keyReceived(const Contact &contact, const QString &keyType, const QByteArray &key);

};

#endif // ENCRYPTION_PROVIDER_H
