/*
 * %kadu copyright begin%
 * Copyright 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2008 Michał Podsiadlik (michal@kadu.net)
 * Copyright 2008, 2009, 2010, 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2010 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#include <QtCore/QFile>

#include "accounts/account.h"
#include "protocols/protocol.h"
#include "protocols/services/chat-service.h"

#include "antistring-notification.h"

#include "antistring.h"

Antistring * Antistring::Instance = 0;

void Antistring::createInstance()
{
	if (!Instance)
		Instance = new Antistring();
}

void Antistring::destroyInstance()
{
	delete Instance;
	Instance = 0;
}

Antistring::Antistring()
{
	triggerAllAccountsRegistered();
}

Antistring::~Antistring()
{
	triggerAllAccountsUnregistered();
}

ChatService * Antistring::chatService(Account account) const
{
	if (!account.protocolHandler())
		return 0;

	return account.protocolHandler()->chatService();
}

void Antistring::accountRegistered(Account account)
{
	ChatService *accountChatService = chatService(account);
	if (!accountChatService)
		return;

	connect(accountChatService, SIGNAL(filterIncomingMessage(Chat, Contact, QString &, time_t, bool &)),
			this, SLOT(filterIncomingMessage(Chat, Contact, QString &, time_t, bool &)));
}

void Antistring::accountUnregistered(Account account)
{
	ChatService *accountChatService = chatService(account);
	if (!accountChatService)
		return;

	disconnect(accountChatService, SIGNAL(filterIncomingMessage(Chat, Contact, QString &, time_t, bool &)),
			this, SLOT(filterIncomingMessage(Chat, Contact, QString &, time_t, bool &)));
}

void Antistring::filterIncomingMessage(Chat chat, Contact sender, QString &message, time_t time, bool &ignore)
{
	Q_UNUSED(time)

	if (!Configuration.enabled())
		return;

	if (points(message) < 3)
		return;

	AntistringNotification::notifyStringReceived(chat);

	ChatService *accountChatService = chatService(chat.chatAccount());
	if (accountChatService)
		accountChatService->sendMessage(chat, Configuration.returnMessage(), true);

	if (Configuration.messageStop())
		ignore = true;

	if (Configuration.logMessage())
		writeLog(sender, message);
}

void Antistring::writeLog(Contact sender, const QString &message)
{
	QFile logFile(Configuration.logFile());

	if (!logFile.exists())
	{
		if (!logFile.open(QFile::WriteOnly))
			return;

		QTextStream stream(&logFile);
		stream << tr("     DATA AND TIME      ::   ID   ::    MESSAGE\n") <<
		             "====================================================\n";

		logFile.close();
	}

	if (!logFile.open(QFile::WriteOnly | QFile::Append))
		return;

	QTextStream stream(&logFile);
	stream << QDateTime::currentDateTime().toString() << " :: " << sender.id() << " :: " <<
			message << "\n";
	logFile.close();
}

int Antistring::points(const QString &message)
{
	int result = 0;

	if (message.length() > 600)
		result++;

	foreach (const ConditionPair &condition, Configuration.conditions())
		if (message.indexOf(QRegExp(condition.first)) >= 0)
			result += condition.second;

	return result;
}
