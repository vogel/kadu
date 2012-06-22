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

#include "gadu-avatar-fetcher.h"

GaduAvatarFetcher::GaduAvatarFetcher(QObject *parent) :
		QObject(parent), RedirectCount(0)
{
	NetworkAccessManager = new QNetworkAccessManager(this);
}

GaduAvatarFetcher::~GaduAvatarFetcher()
{
}

void GaduAvatarFetcher::done(QPixmap avatar)
{
	emit avatarFetched(true, avatar);
	deleteLater();
}

void GaduAvatarFetcher::failed()
{
	emit avatarFetched(false, QPixmap());
	deleteLater();
}

void GaduAvatarFetcher::fetchAvatar(const QString &id)
{
	fetch(QString("http://avatars.gg.pl/%1").arg(id));
}

void GaduAvatarFetcher::requestFinished()
{
	QVariant redirect = Reply->attribute(QNetworkRequest::RedirectionTargetAttribute);
	Reply->deleteLater();

	if (redirect.isNull())
	{
		parseReply();
		return;
	}

	if (RedirectCount > 5)
	{
		failed();
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

	done(pixmap);
}
