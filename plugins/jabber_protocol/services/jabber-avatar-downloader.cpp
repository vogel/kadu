/*
 * %kadu copyright begin%
 * Copyright 2013 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2012, 2013, 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * %kadu copyright end%
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2009, 2010 Piotr Galiszewski (piotrgaliszewski@gmail.com)
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

#include "jabber-avatar-downloader.h"

#include "services/jabber-vcard-downloader.h"
#include "services/jabber-vcard-service.h"
#include "jabber-protocol.h"

#include <qxmpp/QXmppVCardIq.h>

JabberAvatarDownloader::JabberAvatarDownloader(JabberVCardService *vCardService, QObject *parent) :
		AvatarDownloader(parent), VCardService(vCardService)
{
}

JabberAvatarDownloader::~JabberAvatarDownloader()
{
}

void JabberAvatarDownloader::done(QImage avatar)
{
	emit avatarDownloaded(true, avatar);
	deleteLater();
}

void JabberAvatarDownloader::failed()
{
	emit avatarDownloaded(false, QImage());
	deleteLater();
}

void JabberAvatarDownloader::downloadAvatar(const QString &id)
{
	if (!VCardService || id.isEmpty())
	{
		failed();
		return;
	}

	JabberVCardDownloader *vCardDownloader = VCardService.data()->createVCardDownloader();
	if (!vCardDownloader)
	{
		failed();
		return;
	}

	connect(vCardDownloader, SIGNAL(vCardDownloaded(bool,QXmppVCardIq)), this, SLOT(vCardDownloaded(bool,QXmppVCardIq)));
	vCardDownloader->downloadVCard(id);
}

void JabberAvatarDownloader::vCardDownloaded(bool ok, const QXmppVCardIq &vCard)
{
	if (!ok)
	{
		failed();
		return;
	}

	done(QImage::fromData(vCard.photo()));
}

#include "moc_jabber-avatar-downloader.cpp"
