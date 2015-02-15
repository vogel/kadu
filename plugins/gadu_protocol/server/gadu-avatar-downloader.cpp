/*
 * %kadu copyright begin%
 * Copyright 2012, 2013 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2012, 2013, 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "gadu-avatar-downloader.h"

GaduAvatarDownloader::GaduAvatarDownloader(QObject *parent) :
		AvatarDownloader{parent},
		Reply{},
		RedirectCount{0}
{
	NetworkAccessManager = new QNetworkAccessManager(this);
}

GaduAvatarDownloader::~GaduAvatarDownloader()
{
}

void GaduAvatarDownloader::done(QImage avatar)
{
	emit avatarDownloaded(true, avatar);
	deleteLater();
}

void GaduAvatarDownloader::failed()
{
	emit avatarDownloaded(false, QImage());
	deleteLater();
}

void GaduAvatarDownloader::downloadAvatar(const QString &id)
{
	fetch(QString("http://avatars.gg.pl/%1/s,big").arg(id));
}

void GaduAvatarDownloader::requestFinished()
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

void GaduAvatarDownloader::fetch(const QString &url)
{
	QNetworkRequest request;
	request.setUrl(url);

	Reply = NetworkAccessManager->get(request);
	connect(Reply, SIGNAL(finished()), this, SLOT(requestFinished()));
}

void GaduAvatarDownloader::parseReply()
{
	QByteArray data = Reply->readAll();

	if (!data.isEmpty())
		done(QImage::fromData(data));
	else
		done(QImage());
}

#include "moc_gadu-avatar-downloader.cpp"
