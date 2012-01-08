/*
 * %kadu copyright begin%
 * Copyright 2009, 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2009, 2009, 2010, 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2011 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkReply>
#include <QtNetwork/QNetworkRequest>

#include "accounts/account.h"
#include "avatars/avatar-manager.h"
#include "avatars/avatar.h"

#include "server/gadu-avatar-data-parser.h"

#include "gadu-avatar-fetcher.h"

GaduAvatarFetcher::GaduAvatarFetcher(Contact contact, QObject *parent) :
		QObject(parent), MyContact(contact), RedirectCount(0)
{
	NetworkAccessManager = new QNetworkAccessManager(this);
}

void GaduAvatarFetcher::done()
{
	emit avatarFetched(MyContact, true);
}

void GaduAvatarFetcher::failed()
{
	emit avatarFetched(MyContact, false);
}

void GaduAvatarFetcher::fetchAvatar()
{
	fetch(QString("http://avatars.gg.pl/%1").arg(MyContact.id()));
}

void GaduAvatarFetcher::requestFinished()
{
	QVariant redirect = Reply->attribute(QNetworkRequest::RedirectionTargetAttribute);
	Reply->deleteLater();

	if (redirect.isNull())
	{
		parseReply();
		deleteLater();
		return;
	}

	if (RedirectCount > 5)
	{
		done();
		deleteLater();
		return;
	}

	RedirectCount++;

	fetch(redirect.toString());
}

void GaduAvatarFetcher::fetch(const QString &url)
{
	QNetworkRequest request;
	request.setUrl(url);

	Reply = NetworkAccessManager->get(request);
	connect(Reply, SIGNAL(finished()), this, SLOT(requestFinished()));
}

void GaduAvatarFetcher::parseReply()
{
	QByteArray data = Reply->readAll();
	QPixmap pixmap;
	if (!data.isEmpty())
		pixmap.loadFromData(data);

	AvatarManager::instance()->byContact(MyContact, ActionCreateAndAdd).setPixmap(pixmap);

	done();
}
