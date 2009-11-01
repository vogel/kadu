/***************************************************************************
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
*                                                                         *
***************************************************************************/

#include <QtNetwork/QHttp>
#include <QtCore/QUrl>

#include "accounts/account.h"
#include "buddies/avatar-manager.h"
#include "misc/path-conversion.h"
#include "tlen.h"
#include "tlen-protocol.h"

#include "tlen-avatar-fetcher.h"

TlenAvatarFetcher::TlenAvatarFetcher(ContactAccountData *contactAccountData, QObject *parent) :
		QObject(parent), MyContactAccountData(contactAccountData)
{
}

void TlenAvatarFetcher::fetchAvatar()
{
	tlen * tlenClient = (dynamic_cast <TlenProtocol *> (MyContactAccountData->account()->protocolHandler()))->client();

	// TODO: clean up, clean access to tlenClient
	// create QString tlen/protocol::avatarGetRequest(QString login), avatarGetRequestMethod()
	// prevent fetch more than one avatar at the same time - contactlistwidget sends requests
	QString login(MyContactAccountData->id());
	login.remove(QString("@tlen.pl"));

	QString type("0");
	QString token(tlenClient->token);

	if ((tlenClient->mmBase()).isEmpty()
	  || (tlenClient->token).isEmpty()
	  || login.isEmpty()
	  || type.isEmpty())
		return;

	QString request("/");
	request.append(tlenClient->MiniMailAvatarGet);
	request.replace(QString("^login^"), login);
	request.replace(QString("^type^"), type);
	request.replace(QString("^token^"), token);
	QUrl address(tlenClient->mmBase());
	MyHttp = new QHttp(address.host(), 80, this);

	connect(MyHttp, SIGNAL(requestFinished(int, bool)),
			this, SLOT(avatarDownloaded(int, bool)));
	MyHttp->get(request, &MyAvatarBuffer);
}

void TlenAvatarFetcher::avatarDownloaded(int id, bool error)
{
	QImage image;
	QPixmap pixmap;

	// 200 OK and buffer not empty
	if (!MyAvatarBuffer.data().isEmpty() && (MyHttp->lastResponse()).statusCode() == 200)
	{
		//MyContactAccountData->avatar().setNextUpdate(QDateTime::fromTime_t(QDateTime::currentDateTime().toTime_t() + 7200));
		emit avatarFetched(MyContactAccountData, MyAvatarBuffer.buffer());
	}


	deleteLater();
}
