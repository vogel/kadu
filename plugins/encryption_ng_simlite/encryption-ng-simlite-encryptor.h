/*
 * %kadu copyright begin%
 * Copyright 2011, 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#ifndef ENCRYPTION_NG_SIMLITE_ENCRYPTOR_H
#define ENCRYPTION_NG_SIMLITE_ENCRYPTOR_H

#include <QtCore/QDataStream>
#include <QtCore/QObject>
#include <QtCrypto>

#include "contacts/contact.h"

#include "plugins/encryption_ng/encryptor.h"

class Key;

class EncryptioNgSimliteEncryptor : public Encryptor
{
	Q_OBJECT

	Contact MyContact;
	QCA::PublicKey EncodingKey;
	bool Valid;

	void updateKey();
	QCA::PublicKey getPublicKey(const Key &key);

private slots:
	void keyUpdated(const Key &key);

public:
	EncryptioNgSimliteEncryptor(const Contact &contact, EncryptionProvider *provider, QObject *parent = 0);
	virtual ~EncryptioNgSimliteEncryptor();

	virtual RawMessage encrypt(const RawMessage &rawMessage) override;

	bool isValid() { return Valid; }

};

#endif // ENCRYPTION_NG_SIMLITE_ENCRYPTOR_H
