/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef GADU_OPEN_CHAT_WITH_RUNNER_H
#define GADU_OPEN_CHAT_WITH_RUNNER_H

#include "accounts/account.h"

#include "gui/windows/open-chat-with/open-chat-with-runner.h"

class GaduOpenChatWithRunner : public OpenChatWithRunner
{
	Account ParentAccount;

	bool validateUserID(const QString &uid);

public:
	GaduOpenChatWithRunner(Account account);
	virtual BuddyList matchingContacts(const QString &query);
	void setAccount(Account account) { ParentAccount = account; }

};

#endif // GADU_OPEN_CHAT_WITH_RUNNER_H
