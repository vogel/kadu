/*
 * %kadu copyright begin%
 * Copyright 2011 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#ifndef DECRYPTOR_H
#define DECRYPTOR_H

#include <QtCore/QObject>

#include "encryption_exports.h"

class Chat;
class EncryptionProvider;
class RawMessage;

class ENCRYPTIONAPI Decryptor : public QObject
{
	Q_OBJECT

	EncryptionProvider *Provider;

public:
	explicit Decryptor(EncryptionProvider *provider, QObject *parent = 0);
	virtual ~Decryptor() {}

	EncryptionProvider * provider() { return Provider; }

	virtual RawMessage decrypt(const RawMessage &rawMessage, Chat chat, bool *ok = 0) = 0;

};

#endif // DECRYPTOR_H
