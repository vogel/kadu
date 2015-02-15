/*
 * %kadu copyright begin%
 * Copyright 2011, 2012, 2014 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#ifndef ENCRYPTION_CHAT_DATA_H
#define ENCRYPTION_CHAT_DATA_H

#include <QtCore/QObject>
#include <QtCore/QPointer>
#include <QtWidgets/QAction>

#include "chat/chat.h"

class Decryptor;
class Encryptor;

class EncryptionChatData : public QObject
{
	Q_OBJECT

private:
	Chat MyChat;
	QPointer<Encryptor> ChatEncryptor;
	QPointer<Decryptor> ChatDecryptor;

	bool Encrypt;

	void importEncrypt();

public:
	explicit EncryptionChatData(const Chat &chat, QObject *parent);
	virtual ~EncryptionChatData();

	void setEncrypt(bool encrypt);
	bool encrypt() const { return Encrypt; }

	void setLastEncryptionProviderName(const QString &lastEncryptionProviderName);
	QString lastEncryptionProviderName() const;

	void setEncryptor(Encryptor *encryptor);
	Encryptor * encryptor() const;

	void setDecryptor(Decryptor *decryptor);
	Decryptor * decryptor() const;

};

#endif // ENCRYPTION_CHAT_DATA_H
