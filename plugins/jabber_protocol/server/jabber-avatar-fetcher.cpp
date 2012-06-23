/*
 * %kadu copyright begin%
 * Copyright 2009, 2010, 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009, 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2009, 2010, 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2010, 2011 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#include "server/jabber-avatar-pep-fetcher.h"
#include "server/jabber-avatar-vcard-fetcher.h"
#include "services/jabber-pep-service.h"
#include "services/jabber-vcard-service.h"

#include "jabber-avatar-fetcher.h"

JabberAvatarFetcher::JabberAvatarFetcher(const QString &id, JabberPepService *pepService, XMPP::JabberVCardService *vCardService, QObject *parent) :
		QObject(parent), Id(id), PepService(pepService), VCardService(vCardService)
{
}

JabberAvatarFetcher::~JabberAvatarFetcher()
{
}

void JabberAvatarFetcher::failed()
{
	emit avatarFetched(false, QPixmap());
	deleteLater();
}

void JabberAvatarFetcher::fetchAvatarPEP()
{
	if (!PepService)
	{
		failed();
		return;
	}

	JabberAvatarPepFetcher *pepFetcher = new JabberAvatarPepFetcher(PepService.data(), this);
	connect(pepFetcher, SIGNAL(avatarFetched(bool,QPixmap)), this, SLOT(pepAvatarFetched(bool,QPixmap)));
	pepFetcher->fetchAvatar(Id);
}

void JabberAvatarFetcher::fetchAvatarVCard()
{
	if (!VCardService)
	{
		failed();
		return;
	}

	JabberAvatarVCardFetcher *vcardFetcher = new JabberAvatarVCardFetcher(Id, VCardService.data(), this);
	connect(vcardFetcher, SIGNAL(avatarFetched(bool,QPixmap)), this, SLOT(avatarFetchedSlot(bool,QPixmap)));
	vcardFetcher->fetchAvatar();
}

void JabberAvatarFetcher::pepAvatarFetched(bool ok, QPixmap avatar)
{
	if (ok)
	{
		emit avatarFetched(ok, avatar);
		deleteLater();
		return;
	}

	// do a fallback to vcard
	fetchAvatarVCard();
}

void JabberAvatarFetcher::avatarFetchedSlot(bool ok, QPixmap avatar)
{
	emit avatarFetched(ok, avatar);
	deleteLater();
}

void JabberAvatarFetcher::fetchAvatar()
{
	if (PepService && PepService.data()->enabled())
		fetchAvatarPEP();
	else
		fetchAvatarVCard();
}
