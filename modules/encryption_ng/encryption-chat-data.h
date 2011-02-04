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

#ifndef ENCRYPTION_CHAT_DATA_H
#define ENCRYPTION_CHAT_DATA_H

#include <QtCore/QObject>
#include <QtGui/QAction>

#include "chat/chat.h"
#include "storage/module-data.h"

#undef Property
#define Property(type, name, capitalized_name) \
	type name() { ensureLoaded(); return capitalized_name; } \
	void set##capitalized_name(const type &name) { ensureLoaded(); capitalized_name = name; }

class Decryptor;
class Encryptor;

class EncryptionChatData : public ModuleData
{
	Q_OBJECT

	Encryptor *ChatEncryptor;
	Decryptor *ChatDecryptor;

	bool Encrypt;

	bool importEncrypt();

private slots:
	void encryptorDestroyed();
	void decryptorDestroyed();

protected:
	virtual void load();

public:
	explicit EncryptionChatData(const QString &moduleName, StorableObject *parent, QObject *qobjectParent);
	virtual ~EncryptionChatData();

	virtual void store();
	virtual QString name() const;

	Property(bool, encrypt, Encrypt)

	void setEncryptor(Encryptor *encryptor);
	Encryptor * encryptor();

	void setDecryptor(Decryptor *decryptor);
	Decryptor * decryptor();

};

#endif // ENCRYPTION_CHAT_DATA_H
