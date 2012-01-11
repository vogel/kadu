/*
 * %kadu copyright begin%
 * Copyright 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2008, 2010, 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2011 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#ifndef ANTISTRING_H
#define ANTISTRING_H

#include "accounts/accounts-aware-object.h"
#include "configuration/configuration-aware-object.h"
#include "gui/windows/main-configuration-window.h"

#include "antistring-configuration.h"

class Chat;
class ChatService;
class Contact;

class Antistring : public QObject, public AccountsAwareObject
{
	Q_OBJECT
	Q_DISABLE_COPY(Antistring)

	static Antistring * Instance;

	AntistringConfiguration Configuration;

	Antistring();
	virtual ~Antistring();

	int points(const QString &message);
	void writeLog(Contact sender, const QString &message);

	ChatService * chatService(Account account) const;

private slots:
	void filterIncomingMessage(Chat chat, Contact sender, QString &message, bool &ignore);

protected:
	virtual void accountRegistered(Account account);
	virtual void accountUnregistered(Account account);

public:
	static void createInstance();
	static void destroyInstance();

	static Antistring * instance() { return Instance; }

	AntistringConfiguration & configuration() { return Configuration; }

};

// for MOC
#include "chat/chat.h"
#include "contacts/contact.h"

#endif // ANTISTRING_H
