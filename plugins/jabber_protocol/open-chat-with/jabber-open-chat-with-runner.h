/*
 * %kadu copyright begin%
 * Copyright 2010 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2010 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2009 Bartłomiej Zimoń (uzi18@o2.pl)
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

#ifndef JABBER_OPEN_CHAT_WITH_RUNNER_H
#define JABBER_OPEN_CHAT_WITH_RUNNER_H

#include "accounts/account.h"

#include "gui/windows/open-chat-with/open-chat-with-runner.h"

class JabberOpenChatWithRunner : public OpenChatWithRunner
{
	Account ParentAccount;

public:
	JabberOpenChatWithRunner(Account account);
	virtual BuddyList matchingContacts(const QString &query);
	void setAccount(Account account) { ParentAccount = account; }

};

#endif // JABBER_OPEN_CHAT_WITH_RUNNER_H
