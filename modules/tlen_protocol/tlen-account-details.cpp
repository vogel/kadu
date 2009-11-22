/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "misc/misc.h"
#include "configuration/xml-configuration-file.h"
#include "gui/windows/open-chat-with/open-chat-with-runner-manager.h"

#include "tlen-protocol.h"
#include "tlen-account-details.h"

TlenAccountDetails::TlenAccountDetails(AccountShared *data) :
		AccountDetails(data)
{
	OpenChatRunner = new TlenOpenChatWithRunner(data);
	OpenChatWithRunnerManager::instance()->registerRunner(OpenChatRunner);
}

TlenAccountDetails::~TlenAccountDetails()
{
	OpenChatWithRunnerManager::instance()->unregisterRunner(OpenChatRunner);
	delete OpenChatRunner;
	OpenChatRunner = 0;
}

void TlenAccountDetails::load()
{
	if (!isValidStorage())
		return;

	AccountDetails::load();
}

void TlenAccountDetails::store()
{
	if (!isValidStorage())
		return;
}
