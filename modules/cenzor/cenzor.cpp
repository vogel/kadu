/*
 * %kadu copyright begin%
 * Copyright 2010 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2010 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2008, 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2008 Michał Podsiadlik (michal@kadu.net)
 * Copyright 2008 Tomasz Rostański (rozteck@interia.pl)
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

#include "protocols/services/chat-service.h"

#include "notify/cenzor-notification.h"

#include "cenzor.h"

Cenzor * Cenzor::Instance = 0;

void Cenzor::createInstance()
{
	if (!Instance)
		Instance = new Cenzor();
}

void Cenzor::destroyInstance()
{
	delete Instance;
	Instance = 0;
}


Cenzor::Cenzor()
{
	triggerAllAccountsRegistered();
}

Cenzor::~Cenzor()
{
	triggerAllAccountsUnregistered();
}

void Cenzor::accountRegistered(Account account)
{
	Protocol *protocol = account.protocolHandler();
	if (!protocol)
		return;

	ChatService *chatService = protocol->chatService();
	if (!chatService)
		return;

	connect(chatService, SIGNAL(filterIncomingMessage(Chat,Contact,QString &,time_t,bool&)),
			this, SLOT(filterIncomingMessage(Chat,Contact,QString &,time_t,bool&)));
}

void Cenzor::accountUnregistered(Account account)
{
	Protocol *protocol = account.protocolHandler();
	if (!protocol)
		return;

	ChatService *chatService = protocol->chatService();
	if (!chatService)
		return;

	disconnect(chatService, SIGNAL(filterIncomingMessage(Chat,Contact,QString &,time_t,bool&)),
			this, SLOT(filterIncomingMessage(Chat,Contact,QString &,time_t,bool&)));
}

void Cenzor::filterIncomingMessage(Chat chat, Contact sender, QString &message, time_t time, bool &ignore)
{
	Q_UNUSED(sender)
	Q_UNUSED(time)

	if (!Configuration.enabled())
		return;

	if (!shouldIgnore(message))
		return;

	ignore = true;

	Account account = chat.chatAccount();

	Protocol *protocol = account.protocolHandler();
	if (!protocol)
		return;

	ChatService *chatService = protocol->chatService();
	if (!chatService)
		return;

	chatService->sendMessage(chat, Configuration.admonition(), true);

	CenzorNotification::notifyCenzored(chat);
}

bool Cenzor::shouldIgnore(const QString &message)
{
	QStringList words = message.split(' ', QString::SkipEmptyParts);

	foreach (const QString &word, words)
	{
		QString lowerWord = word.toLower();
		foreach (const QRegExp &swear, Configuration.swearList())
			if ((swear.indexIn(lowerWord) >= 0) && (!isExclusion(lowerWord)))
				return true;
	}


	return false;
}

bool Cenzor::isExclusion(const QString &word)
{
	foreach (const QRegExp &exclusion, Configuration.exclusionList())
		if (exclusion.indexIn(word) >= 0)
			return true;

	return false;
}
