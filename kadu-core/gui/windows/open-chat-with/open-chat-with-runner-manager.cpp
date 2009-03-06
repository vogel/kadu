/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "contacts/contact.h"

#include "config_file.h"
#include "debug.h"

#include "open-chat-with-runner-manager.h"

OpenChatWithRunnerManager * OpenChatWithRunnerManager::Instance = 0;

OpenChatWithRunnerManager * OpenChatWithRunnerManager::instance()
{
	if (!Instance)
		Instance = new OpenChatWithRunnerManager();

	return Instance;
}

OpenChatWithRunnerManager::OpenChatWithRunnerManager()
{
}

OpenChatWithRunnerManager::~OpenChatWithRunnerManager()
{
}

void OpenChatWithRunnerManager::registerRunner(OpenChatWithRunner *runner)
{
	Runners.append(runner);
}

void OpenChatWithRunnerManager::unregisterRunner(OpenChatWithRunner *runner)
{
	Runners.removeAll(runner);
}

ContactList OpenChatWithRunnerManager::matchingContacts(const QString &query)
{
	kdebugf();

	ContactList matchingContacts;
	foreach (OpenChatWithRunner *runner, Runners)
		matchingContacts += runner->matchingContacts(query);

	return matchingContacts;
}
