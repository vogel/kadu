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

#ifndef ENCRYPTION_PROVIDER_MANAGER_H
#define ENCRYPTION_PROVIDER_MANAGER_H

#include <QtCore/QObject>

#include "chat/chat.h"

class Decryptor;
class Encryptor;
class EncryptionProvider;

class EncryptionProviderManager : public QObject
{
	Q_OBJECT
	Q_DISABLE_COPY(EncryptionProviderManager)

	static EncryptionProviderManager * Instance;

	QList<EncryptionProvider *> Providers;

	EncryptionProviderManager();
	virtual ~EncryptionProviderManager();

private slots:
	void keyReceived(const Contact &contact, const QString &keyType, const QByteArray &keyData);

public:
	static void createInstance();
	static void destroyInstance();

	static EncryptionProviderManager * instance() { return Instance; }

	void registerProvider(EncryptionProvider *provider);
	void unregisterProvider(EncryptionProvider *provider);

	bool canEncrypt(const Chat &chat);
	bool canDecrypt(const Chat &chat);

	Encryptor * encryptor(const Chat &chat);
	Decryptor * decryptor(const Chat &chat);
	Decryptor * decryptorWrapper(const Chat &chat);

signals:
	void canEncryptChanged(const Chat &chat);
	void canDecryptChanged(const Chat &chat);

};

#endif // ENCRYPTION_PROVIDER_MANAGER_H
