/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef JABBER_OPEN_CHAT_WITH_RUNNER_H
#define JABBER_OPEN_CHAT_WITH_RUNNER_H

#include "accounts/account.h"

#include "gui/windows/open-chat-with/open-chat-with-runner.h"

class JabberOpenChatWithRunner : public OpenChatWithRunner
{
	Account ParentAccount;

	bool validateUserID(const QString &uid);

public:
	JabberOpenChatWithRunner(Account account);
	virtual ContactList matchingContacts(const QString &query);
	void setAccount(Account account) { ParentAccount = account; }

};

#endif // JABBER_OPEN_CHAT_WITH_RUNNER_H
