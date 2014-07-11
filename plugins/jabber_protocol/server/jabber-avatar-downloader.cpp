/*
 * %kadu copyright begin%
 * Copyright 2012 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2012, 2013 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2013 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#include <xmpp_client.h>

#include "accounts/account-manager.h"
#include "server/jabber-avatar-pep-downloader.h"
#include "server/jabber-avatar-vcard-downloader.h"
#include "services/jabber-pep-service.h"
#include "services/jabber-vcard-service.h"
#include "jabber-protocol.h"

#include "jabber-avatar-downloader.h"

JabberAvatarDownloader::JabberAvatarDownloader( JabberPepService *pepService, XMPP::JabberVCardService *vCardService, QObject *parent) :
		AvatarDownloader(parent), PepService(pepService), VCardService(vCardService)
{
}

JabberAvatarDownloader::~JabberAvatarDownloader()
{
}

void JabberAvatarDownloader::failed()
{
	emit avatarDownloaded(false, QImage());
	deleteLater();
}

void JabberAvatarDownloader::downloadAvatarPEP()
{
	if (!PepService)
	{
		failed();
		return;
	}

	JabberAvatarPepDownloader *pepDownloader = new JabberAvatarPepDownloader(PepService.data(), this);
	connect(pepDownloader, SIGNAL(avatarDownloaded(bool,QImage)), this, SLOT(pepAvatarDownloaded(bool,QImage)));
	pepDownloader->downloadAvatar(Id);
}

void JabberAvatarDownloader::downloadAvatarVCard()
{
	if (!VCardService)
	{
		failed();
		return;
	}

	JabberAvatarVCardDownloader *vcardDownloader = new JabberAvatarVCardDownloader(VCardService.data(), this);
	connect(vcardDownloader, SIGNAL(avatarDownloaded(bool,QImage)), this, SLOT(vCardAvatarDownloaded(bool,QImage)));
	vcardDownloader->downloadAvatar(Id);
}

void JabberAvatarDownloader::pepAvatarDownloaded(bool ok, QImage avatar)
{
	if (ok)
	{
		emit avatarDownloaded(ok, avatar);
		deleteLater();
		return;
	}

	// do a fallback to vcard
	downloadAvatarVCard();
}

void JabberAvatarDownloader::vCardAvatarDownloaded(bool ok, QImage avatar)
{
	emit avatarDownloaded(ok, avatar);
	deleteLater();
}

void JabberAvatarDownloader::downloadAvatar(const QString &id)
{
	Id = id;

	if (PepService && PepService.data()->enabled())
		downloadAvatarPEP();
	else
		downloadAvatarVCard();
}

#include "moc_jabber-avatar-downloader.cpp"
