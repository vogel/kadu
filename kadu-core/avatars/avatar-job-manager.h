/*
 * %kadu copyright begin%
 * Copyright 2008, 2010 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2008 Michał Podsiadlik (michal@kadu.net)
 * Copyright 2007, 2008, 2009, 2010, 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2011, 2013 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2007, 2008 Dawid Stawiarski (neeo@kadu.net)
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

#ifndef AVATAR_JOB_MANAGER_H
#define AVATAR_JOB_MANAGER_H

#include <QtCore/QObject>
#include <QtCore/QSet>

#include "accounts/accounts-aware-object.h"
#include "avatars/avatar.h"
#include "storage/simple-manager.h"
#include "exports.h"

class Contact;

class KADUAPI AvatarJobManager : public QObject
{
	Q_OBJECT
	Q_DISABLE_COPY(AvatarJobManager)

	static AvatarJobManager *Instance;

	QMutex Mutex;
	bool IsJobRunning;

	QSet<Contact> Jobs;

	AvatarJobManager();
	virtual ~AvatarJobManager();

	QMutex & mutex() { return Mutex; }

	void scheduleJob();

private slots:
	void runJob();
	void jobFinished();

public:
	static AvatarJobManager * instance();

	void addJob(const Contact &contact);

	bool hasJob();
	Contact nextJob();

signals:
	void jobAvailable();

};

#endif // AVATAR_JOB_MANAGER_H
