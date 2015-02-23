/*
 * %kadu copyright begin%
 * Copyright 2013 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2011, 2012, 2013, 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "server/jabber-avatar-downloader.h"
#include "server/jabber-avatar-pep-downloader.h"
#include "server/jabber-avatar-pep-uploader.h"
#include "server/jabber-avatar-uploader.h"
#include "server/jabber-avatar-vcard-downloader.h"
#include "server/jabber-avatar-vcard-uploader.h"
#include "services/jabber-pep-service.h"
#include "services/jabber-vcard-service.h"
#include "jabber-protocol.h"

#include "jabber-avatar-service.h"

JabberAvatarService::JabberAvatarService(Account account, QObject *parent) :
		AvatarService(account, parent)
{
}

JabberAvatarService::~JabberAvatarService()
{
}

void JabberAvatarService::setPepService(JabberPepService *pepService)
{
	PepService = pepService;
}

void JabberAvatarService::setVCardService(JabberVCardService *vCardService)
{
	VCardService = vCardService;
}

AvatarDownloader * JabberAvatarService::createAvatarDownloader()
{/*
	auto protocol = qobject_cast<JabberProtocol *>(account().protocolHandler());
	if (!protocol->isConnected() || !protocol->xmppClient())
		return 0;
	if (!PepService.data() && !VCardService.data())
		return 0;
	if (PepService.data() && !VCardService.data())
		return new JabberAvatarPepDownloader(PepService.data(), this);
	if (!PepService.data() && VCardService.data())
		return new JabberAvatarVCardDownloader(VCardService.data(), this);
	return new JabberAvatarDownloader(PepService.data(), VCardService.data(), this);*/
	return nullptr;
}

AvatarUploader * JabberAvatarService::createAvatarUploader()
{/*
	auto protocol = qobject_cast<JabberProtocol *>(account().protocolHandler());
	if (!protocol->isConnected() || !protocol->xmppClient())
		return 0;
	if (!PepService.data() && !VCardService.data())
		return 0;
	if (PepService.data() && !VCardService.data())
		return new JabberAvatarPepUploader(PepService.data(), this);
	if (!PepService.data() && VCardService.data())
		return new JabberAvatarVCardUploader(VCardService.data(), this);
	return new JabberAvatarUploader(PepService.data(), VCardService.data(), this);*/
	return nullptr;
}

#include "moc_jabber-avatar-service.cpp"
