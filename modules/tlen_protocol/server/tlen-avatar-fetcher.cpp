/*
 * %kadu copyright begin%
 * Copyright 2011 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2009, 2010 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2009, 2010 Bartłomiej Zimoń (uzi18@o2.pl)
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

#include <QtCore/QUrl>

#include "accounts/account.h"
#include "avatars/avatar.h"
#include "avatars/avatar-manager.h"
#include "misc/path-conversion.h"
#include "http_client.h"

#include "tlen.h"
#include "tlen-protocol.h"

#include "tlen-avatar-fetcher.h"

TlenAvatarFetcher::TlenAvatarFetcher(Contact contact, QObject *parent) :
		QObject(parent), MyContact(contact)
{
}

void TlenAvatarFetcher::fetchAvatar()
{
	tlen * tlenClient = (static_cast<TlenProtocol *>(MyContact.contactAccount().protocolHandler()))->client();
	if (!tlenClient)
		return;

	// TODO: clean up, clean access to tlenClient
	// create QString tlen/protocol::avatarGetRequest(QString login), avatarGetRequestMethod()
	// prevent fetch more than one avatar at the same time - contactlistwidget sends requests
	QString login(MyContact.id());
	login.remove(QString("@tlen.pl"));
	// remove resource
	login.truncate(login.indexOf("/"));

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

	MyHttp = new HttpClient();
	MyHttp->setHost(address.host());
	connect(MyHttp, SIGNAL(finished()),
			this, SLOT(avatarDownloaded()));
	connect(MyHttp, SIGNAL(error()),
			this, SLOT(avatarDownloaded()));

	MyHttp->get(request, false);
}

void TlenAvatarFetcher::avatarDownloaded()
{
	QPixmap pixmap;

	// 200 OK and buffer not empty and image can convert
	if (!MyHttp->data().isEmpty() && MyHttp->status() == 200 && pixmap.loadFromData(MyHttp->data()))
	{
		if (MyContact.contactAvatar().isNull())
			MyContact.setContactAvatar(Avatar());

		MyContact.contactAvatar().setNextUpdate(QDateTime::fromTime_t(QDateTime::currentDateTime().toTime_t() + 7200));
		emit avatarFetched(MyContact, MyHttp->data());
	}

	deleteLater();
}
