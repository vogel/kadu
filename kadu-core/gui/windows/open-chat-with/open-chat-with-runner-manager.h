/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef OPEN_CHAT_WITH_RUNNER_MANAGER_H
#define OPEN_CHAT_WITH_RUNNER_MANAGER_H

#include <QtCore/QMap>

#include "open-chat-with-runner.h"

class KADUAPI OpenChatWithRunnerManager : public QObject
{
	Q_DISABLE_COPY(OpenChatWithRunnerManager)

	static OpenChatWithRunnerManager *Instance;

	QList<OpenChatWithRunner *> Runners;

	OpenChatWithRunnerManager();
	~OpenChatWithRunnerManager();

public:
	static OpenChatWithRunnerManager * instance();

	void registerRunner(OpenChatWithRunner *runner);
	void unregisterRunner(OpenChatWithRunner *runner);

	ContactList matchingContacts(const QString &query);

};

#endif // OPEN_CHAT_WITH_RUNNER_MANAGER_H
