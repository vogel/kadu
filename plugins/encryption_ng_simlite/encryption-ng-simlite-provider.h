/*
 * %kadu copyright begin%
 * Copyright 2014 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#ifndef ENCRYPTION_NG_SIMLITE_PROVIDER_H
#define ENCRYPTION_NG_SIMLITE_PROVIDER_H

#include <QtCore/QObject>

#include "accounts/accounts-aware-object.h"

#include "plugins/encryption_ng/keys/key.h"

#include "plugins/encryption_ng/encryption-provider.h"

class EncryptioNgSimliteDecryptor;
class EncryptionNgSimliteMessageFilter;

class EncryptioNgSimliteProvider : public EncryptionProvider, AccountsAwareObject
{
	Q_OBJECT
	Q_DISABLE_COPY(EncryptioNgSimliteProvider)

	static EncryptioNgSimliteProvider *Instance;

	QMap<Account, EncryptioNgSimliteDecryptor *> Decryptors;
	QPointer<EncryptionNgSimliteMessageFilter> MessageFilter;

	EncryptioNgSimliteProvider();
	virtual ~EncryptioNgSimliteProvider();

private slots:
	void keyUpdated(Key key);

protected:
	virtual void accountRegistered(Account account);
	virtual void accountUnregistered(Account account);

public:
	static void createInstance();
	static void destroyInstance();

	static EncryptioNgSimliteProvider * instance() { return Instance; }

	void setMessageFilter(EncryptionNgSimliteMessageFilter *messageFilter);

	virtual QString name() const;
	virtual QString displayName() const;

	virtual bool canDecrypt(const Chat &chat) const;
	virtual bool canEncrypt(const Chat &chat) const;

	virtual Encryptor * acquireEncryptor(const Chat &chat);
	virtual Decryptor * acquireDecryptor(const Chat &chat);

	virtual void releaseEncryptor(const Chat &chat, Encryptor *encryptor);
	virtual void releaseDecryptor(const Chat &chat, Decryptor *decryptor);

};

#endif // ENCRYPTION_NG_SIMLITE_PROVIDER_H
