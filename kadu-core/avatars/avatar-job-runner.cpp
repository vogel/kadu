/*
 * %kadu copyright begin%
 * Copyright 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2010, 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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
#include "contacts/contact.h"
#include "protocols/protocol.h"
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

	service->fetchAvatar(MyContact.id(), this);

	Timer = new QTimer(this);
	connect(Timer, SIGNAL(timeout()), this, SLOT(timeout()));
	Timer->start(15000);
}

void AvatarJobRunner::avatarFetched(bool ok, QPixmap avatar)
{
	if (Timer)
		Timer->stop();

	Avatar contactAvatar = AvatarManager::instance()->byContact(MyContact, ActionCreateAndAdd);
	contactAvatar.setLastUpdated(QDateTime::currentDateTime());
	contactAvatar.setNextUpdate(QDateTime::fromTime_t(QDateTime::currentDateTime().toTime_t() + 7200));
	contactAvatar.setPixmap(avatar);

	emit jobFinished(ok);
	deleteLater();
}

void AvatarJobRunner::timeout()
{
	AvatarService *service = AvatarService::fromAccount(MyContact.contactAccount());
	if (service)
		disconnect(service, 0, this, 0);

	emit jobFinished(false);
	deleteLater();
}
