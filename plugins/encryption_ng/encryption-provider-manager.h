/*
 * %kadu copyright begin%
 * Copyright 2011 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2011, 2012, 2013, 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "encryption-provider.h"

#include "encryption_exports.h"

class Decryptor;
class Encryptor;

class ENCRYPTIONAPI EncryptionProviderManager : public EncryptionProvider
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

	EncryptionProvider * byName(const QString &name) const;

	bool canEncrypt(const Chat &chat) const;
	bool canDecrypt(const Chat &chat) const;

	EncryptionProvider * defaultEncryptorProvider(const Chat &chat) const;

	virtual QString name() const;
	virtual QString displayName() const;

	virtual Encryptor * acquireEncryptor(const Chat &chat);
	virtual Decryptor * acquireDecryptor(const Chat &chat);

	virtual void releaseEncryptor(const Chat &chat, Encryptor *encryptor);
	virtual void releaseDecryptor(const Chat &chat, Decryptor *decryptor);

	const QList<EncryptionProvider *> & providers() const { return Providers; }

signals:
	void providerRegistered(EncryptionProvider *provider);
	void providerUnregistered(EncryptionProvider *provider);

};

#endif // ENCRYPTION_PROVIDER_MANAGER_H
