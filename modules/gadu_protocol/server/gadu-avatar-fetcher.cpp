/***************************************************************************
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
*                                                                         *
***************************************************************************/
#include <QtCore/QDebug>
#include <QtXml/QDomDocument>

#include "gadu-avatar-fetcher.h"
#include "misc/path-conversion.h"

void GaduAvatarFetcher::fetchAvatar(ContactAccountData *contactAccountData)
{
	cad = contactAccountData;
	h = new QHttp("api.gadu-gadu.pl");
	connect(h, SIGNAL(requestFinished(int, bool)), this, SLOT(requestFinished(int, bool)));
	h->get("/avatars/" + contactAccountData->id() + "/0.xml", &buff);
}

void GaduAvatarFetcher::requestFinished(int id, bool error)
{
	QString response(buff.data());

	if (response.isEmpty())
		return;

	QDomDocument document;
	document.setContent(buff.data());

	QString user = document.elementsByTagName("user").at(0).toElement().attribute("uin");

	QDomNode avatar = document.elementsByTagName("avatar").at(0);

// 	QDateTime timestamp = QDateTime::fromString(avatar.firstChildElement("timestamp").text());
// 	if (cad->avatar().lastUpdated() == timestamp)
// 		return;
// 	cad->avatar().setLastUpdated(timestamp);

	QString response2 = avatar.firstChildElement("smallAvatar").text();
	QUrl url = QUrl::fromEncoded(QByteArray().append(response2));

	QHttp *http = new QHttp();
	http->setHost(url.host(), url.port(80));
	file = new QFile(ggPath(cad->id() + ".jpg"));
	file->open(QIODevice::WriteOnly);
	connect(http, SIGNAL(requestFinished(int, bool)), this, SLOT(avatarDownloaded(int, bool)));
	http->get(url.path(), file);
}

void GaduAvatarFetcher::avatarDownloaded(int id, bool error)
{
	file->close(); 
	QImage image;
	image.load(ggPath(cad->id() + ".jpg"));
	QPixmap pixmap = QPixmap::fromImage(image);
	cad->avatar().setPixmap(pixmap);
	emit avatarFetched(cad, pixmap);
}
