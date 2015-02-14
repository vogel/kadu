/*
 * %kadu copyright begin%
 * Copyright 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009, 2010 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2009 Bartłomiej Zimoń (uzi18@o2.pl)
 * Copyright 2004 Adrian Smarzewski (adrian@kadu.net)
 * Copyright 2007, 2008, 2009, 2010, 2011, 2013 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2010, 2011, 2013 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2004, 2006 Marcin Ślusarz (joi@kadu.net)
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

#include <QtCore/QTimer>

#include "avatars/avatar-job-runner.h"
#include "contacts/contact.h"

#include "avatar-job-manager.h"
#include <core/application.h>
#include <configuration/deprecated-configuration-api.h>

AvatarJobManager * AvatarJobManager::Instance = 0;

AvatarJobManager * AvatarJobManager::instance()
{
	if (!Instance)
		Instance = new AvatarJobManager();

	return Instance;
}

AvatarJobManager::AvatarJobManager() :
		Mutex(QMutex::Recursive), IsJobRunning(false)
{
}

AvatarJobManager::~AvatarJobManager()
{
}

void AvatarJobManager::scheduleJob()
{
	QMutexLocker locker(&mutex());

	if (!IsJobRunning && hasJob())
		// run it in next even cycle
		// this is for reccursion prevention, so we save on stack memory
		QTimer::singleShot(0, this, SLOT(runJob()));
}

void AvatarJobManager::runJob()
{
	QMutexLocker locker(&mutex());

	if (IsJobRunning)
		return;

	if (!hasJob())
		return;

	if (!Application::instance()->configuration()->deprecatedApi()->readBoolEntry("Look", "ShowAvatars", true))
		return;

	IsJobRunning = true;

	Contact contact = nextJob();
	AvatarJobRunner *runner = new AvatarJobRunner(contact, this);
	connect(runner, SIGNAL(jobFinished(bool)), this, SLOT(jobFinished()));
	runner->runJob();
}

void AvatarJobManager::jobFinished()
{
	QMutexLocker locker(&mutex());

	IsJobRunning = false;
	scheduleJob();
}

void AvatarJobManager::addJob(const Contact &contact)
{
	QMutexLocker locker(&mutex());

	if (!contact)
		return;

	Jobs.insert(contact);
	scheduleJob();
}

bool AvatarJobManager::hasJob()
{
	QMutexLocker locker(&mutex());

	return !Jobs.isEmpty();
}

Contact AvatarJobManager::nextJob()
{
	QMutexLocker locker(&mutex());

	if (!hasJob())
		return Contact::null;

	Contact job = *Jobs.constBegin();
	Jobs.remove(job);

	return job;
}

#include "moc_avatar-job-manager.cpp"
