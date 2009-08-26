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

#include "accounts/account.h"
#include "contacts/avatar-manager.h"
#include "misc/path-conversion.h"

#include "gadu-avatar-fetcher.h"

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

	QDateTime timestamp = QDateTime::fromString(avatar.firstChildElement("timestamp").text());

	QString response2 = avatar.firstChildElement("smallAvatar").text();

	/* Do not cache empty avatars */
	if (response2.contains("avatar-empty.gif"))
	{
		return;
	}

	if (cad->avatar().lastUpdated() == timestamp)
		return;
	cad->avatar().setLastUpdated(timestamp);

	QUrl url = QUrl::fromEncoded(QByteArray().append(response2));

	QHttp *http = new QHttp();
	http->setHost(url.host(), url.port(80));
	file = new QFile(ggPath("avatars/") + QString("%1-%2").arg(cad->contact().uuid().toString(), cad->account()->uuid().toString()));
	file->open(QIODevice::WriteOnly);
	connect(http, SIGNAL(requestFinished(int, bool)), this, SLOT(avatarDownloaded(int, bool)));
	http->get(url.path(), file);
}

void GaduAvatarFetcher::avatarDownloaded(int id, bool error)
{
	file->close(); 
	QImage image;
	image.load(ggPath("avatars/") + QString("%1-%2").arg(cad->contact().uuid().toString(), cad->account()->uuid().toString()));
	QPixmap pixmap = QPixmap::fromImage(image);
	cad->avatar().setPixmap(pixmap);
	emit avatarFetched(cad, pixmap);
}
