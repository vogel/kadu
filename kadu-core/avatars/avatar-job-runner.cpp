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

AvatarService * AvatarJobRunner::avatarService(const Account &account)
{
	Protocol *protocol = account.protocolHandler();
	if (!protocol)
		return 0;

	return protocol->avatarService();
}

AvatarService * AvatarJobRunner::avatarService(const Contact &contact)
{
	Account account = contact.contactAccount();
	if (account.isNull())
		return 0;

	return avatarService(account);
}

void AvatarJobRunner::runJob()
{
	AvatarService *service = avatarService(MyContact);
	if (!service)
	{
		emit jobFinished(false);
		deleteLater();

		return;
	}

	connect(service, SIGNAL(avatarFetched(Contact,bool)),
			this, SLOT(avatarFetched(Contact,bool)));
	service->fetchAvatar(MyContact);

	Timer = new QTimer(this);
	connect(Timer, SIGNAL(timeout()), this, SLOT(timeout()));
	Timer->start(15000);
}

void AvatarJobRunner::avatarFetched(Contact contact, bool ok)
{
	if (MyContact == contact)
	{
		if (Timer)
			Timer->stop();

		emit jobFinished(ok);
		deleteLater();
	}
}

void AvatarJobRunner::timeout()
{
	AvatarService *service = avatarService(MyContact);
	if (service)
		disconnect(service, SIGNAL(avatarFetched(Contact,bool)),
				this, SLOT(avatarFetched(Contact,bool)));

	emit jobFinished(false);
	deleteLater();
}
