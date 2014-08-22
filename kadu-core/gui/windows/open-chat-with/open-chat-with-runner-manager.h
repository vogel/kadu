/*
 * %kadu copyright begin%
 * Copyright 2008, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2007, 2008, 2009, 2010, 2011, 2013 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#ifndef OPEN_CHAT_WITH_RUNNER_MANAGER_H
#define OPEN_CHAT_WITH_RUNNER_MANAGER_H

#include <QtCore/QMap>

#include "open-chat-with-runner.h"

class KADUAPI OpenChatWithRunnerManager : public QObject
{
	Q_OBJECT
	Q_DISABLE_COPY(OpenChatWithRunnerManager)

	static OpenChatWithRunnerManager *Instance;

	QList<OpenChatWithRunner *> Runners;

	OpenChatWithRunnerManager();
	~OpenChatWithRunnerManager();

public:
	static OpenChatWithRunnerManager * instance();

	void registerRunner(OpenChatWithRunner *runner);
	void unregisterRunner(OpenChatWithRunner *runner);

	BuddyList matchingContacts(const QString &query);

};

#endif // OPEN_CHAT_WITH_RUNNER_MANAGER_H
