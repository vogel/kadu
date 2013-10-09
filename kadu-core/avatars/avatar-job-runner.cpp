/*
 * %kadu copyright begin%
 * Copyright 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2010, 2011, 2012 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2012, 2013 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#include "avatars/avatar-manager.h"
#include "avatars/avatar.h"
#include "protocols/protocol.h"
#include "protocols/services/avatar-downloader.h"
#include "protocols/services/avatar-service.h"

#include "avatar-job-runner.h"

AvatarJobRunner::AvatarJobRunner(const Contact &contact, QObject *parent) :
		QObject(parent), MyContact(contact), Timer(0)
{
}

AvatarJobRunner::~AvatarJobRunner()
{
}

void AvatarJobRunner::runJob()
{
	AvatarService *service = AvatarService::fromAccount(MyContact.contactAccount());
	if (!service)
	{
		emit jobFinished(false);
		deleteLater();

		return;
	}

	AvatarDownloader *avatarDownloader = service->createAvatarDownloader();
	if (!avatarDownloader)
	{
		emit jobFinished(false);
		deleteLater();

		return;
	}

	connect(avatarDownloader, SIGNAL(avatarDownloaded(bool,QImage)), this, SLOT(avatarDownloaded(bool,QImage)));
	avatarDownloader->downloadAvatar(MyContact.id());

	Timer = new QTimer(this);
	connect(Timer, SIGNAL(timeout()), this, SLOT(timeout()));
	Timer->start(15000);
}

void AvatarJobRunner::avatarDownloaded(bool ok, QImage avatar)
{
	if (Timer)
		Timer->stop();

	Avatar contactAvatar = AvatarManager::instance()->byContact(MyContact, ActionCreateAndAdd);
	contactAvatar.setLastUpdated(QDateTime::currentDateTime());
	contactAvatar.setNextUpdate(QDateTime::fromTime_t(QDateTime::currentDateTime().toTime_t() + 7200));
	contactAvatar.setPixmap(QPixmap::fromImage(avatar));

	emit jobFinished(ok);
	deleteLater();
}

void AvatarJobRunner::timeout()
{
	emit jobFinished(false);
	deleteLater();
}

#include "moc_avatar-job-runner.cpp"
