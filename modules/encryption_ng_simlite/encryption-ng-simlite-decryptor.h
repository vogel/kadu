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

#ifndef ENCRYPTION_NG_SIMLITE_DECRYPTOR_H
#define ENCRYPTION_NG_SIMLITE_DECRYPTOR_H

#include <QtCore/QObject>
#include <QtCrypto>

#include "modules/encryption_ng/decryptor.h"

class Key;

class EncryptioNgSimliteDecryptor : public Decryptor
{
	Q_OBJECT

	QCA::PrivateKey DecodingKey;
	bool Valid;

	QCA::PrivateKey getPrivateKey(const Key &key);

public:
	EncryptioNgSimliteDecryptor(const Key &key);
	virtual ~EncryptioNgSimliteDecryptor();

	virtual QByteArray decrypt(const QByteArray &data);

};

#endif // ENCRYPTION_NG_SIMLITE_DECRYPTOR_H
