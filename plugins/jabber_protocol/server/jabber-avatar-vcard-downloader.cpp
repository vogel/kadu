/*
 * %kadu copyright begin%
 * Copyright 2009, 2010, 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009, 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2009, 2010, 2011, 2012, 2013 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2010, 2011, 2013 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#include <xmpp_vcard.h>

#include "services/jabber-vcard-downloader.h"
#include "services/jabber-vcard-service.h"
#include "jabber-protocol.h"

#include "jabber-avatar-vcard-downloader.h"

JabberAvatarVCardDownloader::JabberAvatarVCardDownloader(XMPP::JabberVCardService *vCardService, QObject *parent) :
		AvatarDownloader(parent), VCardService(vCardService)
{
}

JabberAvatarVCardDownloader::~JabberAvatarVCardDownloader()
{
}

void JabberAvatarVCardDownloader::done(QImage avatar)
{
	emit avatarDownloaded(true, avatar);
	deleteLater();
}

void JabberAvatarVCardDownloader::failed()
{
	emit avatarDownloaded(false, QImage());
	deleteLater();
}

void JabberAvatarVCardDownloader::downloadAvatar(const QString &id)
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

	connect(vCardDownloader, SIGNAL(vCardDownloaded(bool,XMPP::VCard)), this, SLOT(vCardDownloaded(bool,XMPP::VCard)));
	vCardDownloader->downloadVCard(id);
}

void JabberAvatarVCardDownloader::vCardDownloaded(bool ok, XMPP::VCard vCard)
{
	if (!ok)
	{
		failed();
		return;
	}

	done(QImage::fromData(vCard.photo()));
}

#include "moc_jabber-avatar-vcard-downloader.cpp"
