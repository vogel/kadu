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

#ifndef ENCRYPTION_NG_CAESAR_ENCRYPTOR_H
#define ENCRYPTION_NG_CAESAR_ENCRYPTOR_H

#include "modules/encryption_ng/encryptor.h"

class EncryptionNgCaesarEncryptor : public Encryptor
{
	Q_OBJECT

public:
	virtual ~EncryptionNgCaesarEncryptor() {}

	virtual QByteArray encrypt(const QByteArray &data);

};

#endif // ENCRYPTION_NG_CAESAR_ENCRYPTOR_H
