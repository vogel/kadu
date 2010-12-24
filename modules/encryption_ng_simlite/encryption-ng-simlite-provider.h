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

#ifndef ENCRYPTION_NG_SIMLITE_PROVIDER_H
#define ENCRYPTION_NG_SIMLITE_PROVIDER_H

#include <QtCore/QObject>

#include "accounts/accounts-aware-object.h"

#include "modules/encryption_ng/keys/key.h"

#include "modules/encryption_ng/encryption-provider.h"

class EncryptioNgSimliteDecryptor;

class EncryptioNgSimliteProvider : public EncryptionProvider, AccountsAwareObject
{
	Q_OBJECT
	Q_DISABLE_COPY(EncryptioNgSimliteProvider)

	static EncryptioNgSimliteProvider *Instance;

	QMap<Account, EncryptioNgSimliteDecryptor *> Decryptors;

	EncryptioNgSimliteProvider();
	virtual ~EncryptioNgSimliteProvider();

private slots:
	void keyUpdated(Key key);

	void filterRawIncomingMessage(Chat chat, Contact sender, QByteArray &message, bool &ignore);

protected:
	virtual void accountRegistered(Account account);
	virtual void accountUnregistered(Account account);

public:
	static void createInstance();
	static void destroyInstance();

	static EncryptioNgSimliteProvider * instance() { return Instance; }

	virtual bool canDecrypt(const Chat &chat);
	virtual bool canEncrypt(const Chat &chat);

	virtual Encryptor * acquireEncryptor(const Chat &chat);
	virtual Decryptor * acquireDecryptor(const Chat &chat);

	virtual void releaseEncryptor(const Chat &chat, Encryptor *encryptor);
	virtual void releaseDecryptor(const Chat &chat, Decryptor *decryptor);

};

#endif // ENCRYPTION_NG_SIMLITE_PROVIDER_H
