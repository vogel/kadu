/*
 * %kadu copyright begin%
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2009, 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2009, 2010 Piotr Galiszewski (piotrgaliszewski@gmail.com)
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

#include <QtCore/QDateTime>
#include <QtCore/QDir>
#include <QtCore/QFile>
#include <QtCore/QLatin1String>
#include <QtCore/QUrl>
#include <QtNetwork/QHttp>
#include <QtXml/QDomDocument>

#include "accounts/account.h"
#include "buddies/avatar.h"
#include "buddies/avatar-manager.h"
#include "buddies/avatar-shared.h"
#include "misc/path-conversion.h"

#include "server/gadu-avatar-data-parser.h"

#include "gadu-avatar-fetcher.h"

GaduAvatarFetcher::GaduAvatarFetcher(Contact contact, QObject *parent) :
		QObject(parent), MyContact(contact)
{
}


void GaduAvatarFetcher::fetchAvatar()
{
	MyHttp = new QHttp("api.gadu-gadu.pl", 80, this);
	connect(MyHttp, SIGNAL(requestFinished(int, bool)),
			this, SLOT(requestFinished(int, bool)));
	MyHttp->get("/avatars/" + MyContact.id() + "/0.xml", &MyBuffer);
}

void GaduAvatarFetcher::requestFinished(int id, bool error)
{
	Q_UNUSED(id)

	if (error)
		return;

	GaduAvatarDataParser parser(&MyBuffer, MyContact.id());

	if (!parser.isValid())
	{
		deleteLater();
		return;
	}

	if (parser.isBlank())
	{
		// clear avatar data
		emit avatarFetched(MyContact, QByteArray());
		deleteLater();
		return;
	}

	if (MyContact.contactAvatar().isNull())
		MyContact.setContactAvatar(Avatar::create());

	if (MyContact.contactAvatar().lastUpdated() == parser.timestamp())
	{
		// only if we have file too
		if (!MyContact.contactAvatar().pixmap().isNull())
		{
			// we already have this file, no need to update
			deleteLater();
			return;
		}
	}

	MyContact.contactAvatar().setNextUpdate(QDateTime::fromTime_t(QDateTime::currentDateTime().toTime_t() + parser.delay()));

	MyContact.contactAvatar().setLastUpdated(parser.timestamp());
	AvatarManager::instance()->addItem(MyContact.contactAvatar());

	QUrl url = parser.avatarUrl();

	QHttp *imageFetchHttp = new QHttp(url.host(), 80, this);

	connect(imageFetchHttp, SIGNAL(requestFinished(int, bool)),
			this, SLOT(avatarDownloaded(int, bool)));
			imageFetchHttp->get(url.path(), &AvatarBuffer);
}

void GaduAvatarFetcher::avatarDownloaded(int id, bool error)
{
	Q_UNUSED(id)
	Q_UNUSED(error)

	emit avatarFetched(MyContact, AvatarBuffer.buffer());

	deleteLater();
}
