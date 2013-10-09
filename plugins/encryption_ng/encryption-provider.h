/*
 * %kadu copyright begin%
 * Copyright 2011 Tomasz Rostanski (rozteck@interia.pl)
 * Copyright 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2010 Piotr Dąbrowski (ultr@ultr.pl)
 * Copyright 2009 Bartłomiej Zimoń (uzi18@o2.pl)
 * Copyright 2004 Adrian Smarzewski (adrian@kadu.net)
 * Copyright 2007, 2008, 2009, 2010, 2011, 2012 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2011, 2013 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2004, 2006 Marcin Ślusarz (joi@kadu.net)
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

#include <QtCore/QMetaType>
#include <QtCore/QObject>

#include "encryption_exports.h"

class Chat;
class Contact;
class Decryptor;
class Encryptor;

class ENCRYPTIONAPI EncryptionProvider : public QObject
{
	Q_OBJECT

public:
	virtual ~EncryptionProvider() {}

	virtual QString name() const = 0;
	virtual QString displayName() const = 0;

	virtual bool canEncrypt(const Chat &chat) const = 0;
	virtual bool canDecrypt(const Chat &chat) const = 0;

	virtual Encryptor * acquireEncryptor(const Chat &chat) = 0;
	virtual Decryptor * acquireDecryptor(const Chat &chat) = 0;

	virtual void releaseEncryptor(const Chat &chat, Encryptor *encryptor) = 0;
	virtual void releaseDecryptor(const Chat &chat, Decryptor *decryptor) = 0;

signals:
	void canEncryptChanged(const Chat &chat);
	void canDecryptChanged(const Chat &chat);
	void keyReceived(const Contact &contact, const QString &keyType, const QByteArray &key);

};

Q_DECLARE_METATYPE(EncryptionProvider *);

#endif // ENCRYPTION_PROVIDER_H
