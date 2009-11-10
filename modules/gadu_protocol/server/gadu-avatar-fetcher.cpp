/***************************************************************************
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
*                                                                         *
***************************************************************************/

#include <QtCore/QDir>
#include <QtCore/QFile>
#include <QtCore/QUrl>
#include <QtNetwork/QHttp>
#include <QtXml/QDomDocument>

#include "accounts/account.h"
#include "buddies/avatar-manager.h"
#include "misc/path-conversion.h"

#include "gadu-avatar-fetcher.h"

GaduAvatarFetcher::GaduAvatarFetcher(Contact *contact, QObject *parent) :
		QObject(parent), MyContact(contact)
{
}


void GaduAvatarFetcher::fetchAvatar()
{
	MyHttp = new QHttp("api.gadu-gadu.pl", 80, this);
	connect(MyHttp, SIGNAL(requestFinished(int, bool)),
			this, SLOT(requestFinished(int, bool)));
	MyHttp->get("/avatars/" + MyContact->id() + "/0.xml", &MyBuffer);
}

void GaduAvatarFetcher::requestFinished(int id, bool error)
{
	QString response(MyBuffer.data());

	if (response.isEmpty())
	{
		deleteLater();
		return;
	}

	QDomDocument document;
	document.setContent(MyBuffer.data());

	QDomElement resultElement = document.firstChildElement("result");
	if (resultElement.isNull())
	{
		deleteLater();
		return;
	}

	QDomElement usersElement = resultElement.firstChildElement("users");
	if (usersElement.isNull())
	{
		deleteLater();
		return;
	}

	QDomElement userElement = usersElement.firstChildElement("user");
	if (userElement.isNull())
	{
		deleteLater();
		return;
	}

	QString uin = userElement.attribute("uin");
	if (uin.isEmpty())
	{
		deleteLater();
		return;
	}

	QDomElement avatarsElement = userElement.firstChildElement("avatars");
	if (avatarsElement.isNull())
	{
		deleteLater();
		return;
	}

	QDomElement avatarElement = avatarsElement.firstChildElement("avatar");
	if (avatarElement.isNull())
	{
		deleteLater();
		return;
	}

	QDateTime timestamp;
	QDomElement timestampElement = avatarElement.firstChildElement("timestamp");
	if (!timestampElement.isNull())
	{
		timestamp = QDateTime::fromString(timestampElement.text());
		if (MyContact->avatar().lastUpdated() == timestamp)
		{
// 			deleteLater(); TODO: check if file is present
// 			return;
		}
	}

	QDomElement packageDelayElement = avatarElement.firstChildElement("packageDelay");
	if (!packageDelayElement.isNull())
	{
		int delay = packageDelayElement.text().toInt();
		MyContact->avatar().setNextUpdate(QDateTime::fromTime_t(QDateTime::currentDateTime().toTime_t() + delay));
	}

	QDomElement avatarFileElement = avatarElement.firstChildElement("bigAvatar");
	if (avatarFileElement.isNull())
		avatarFileElement = avatarElement.firstChildElement("smallAvatar");
	if (avatarFileElement.isNull())
	{
		deleteLater();
		return;
	}

	QString avatarUrl = avatarFileElement.text();

	// Do not cache empty avatars
	if (avatarUrl.contains("avatar-empty.gif") || avatarUrl.contains("a1.gif"))
	{
		deleteLater();
		return;
	}

	MyContact->avatar().setLastUpdated(timestamp);

	QUrl url = avatarUrl;

	QHttp *imageFetchHttp = new QHttp(url.host(), 80, this);

	connect(imageFetchHttp, SIGNAL(requestFinished(int, bool)),
			this, SLOT(avatarDownloaded(int, bool)));
			imageFetchHttp->get(url.path(), &AvatarBuffer);
}

void GaduAvatarFetcher::avatarDownloaded(int id, bool error)
{
	emit avatarFetched(MyContact, AvatarBuffer.buffer());

	deleteLater();
}
