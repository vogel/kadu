/*
 * %kadu copyright begin%
 * Copyright 2011 Tomasz Rostanski (rozteck@interia.pl)
 * Copyright 2008, 2010 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2008 Michał Podsiadlik (michal@kadu.net)
 * Copyright 2009 Bartłomiej Zimoń (uzi18@o2.pl)
 * Copyright 2004 Adrian Smarzewski (adrian@kadu.net)
 * Copyright 2007, 2008, 2009, 2010, 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2011 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2004, 2006 Marcin Ślusarz (joi@kadu.net)
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

#ifndef JABBER_AVATAR_VCARD_FETCHER_H
#define JABBER_AVATAR_VCARD_FETCHER_H

#include <QtGui/QPixmap>

#include "services/jabber-vcard-fetch-callback.h"

namespace XMPP
{
	class VCard;

	class JabberVCardService;
}

class JabberAvatarVCardFetcher : public QObject, public XMPP::JabberVCardFetchCallback
{
	Q_OBJECT

	QString Id;
	QWeakPointer<XMPP::JabberVCardService> VCardService;

	void done(QPixmap avatar);
	void failed();

protected:
	virtual void vCardFetched(bool ok, const XMPP::VCard &vCard);

public:
	explicit JabberAvatarVCardFetcher(const QString &id, XMPP::JabberVCardService *vCardService, QObject *parent = 0);
	virtual ~JabberAvatarVCardFetcher();

	void fetchAvatar();

signals:
	void avatarFetched(bool ok, QPixmap avatar);

};

#endif // JABBER_AVATAR_VCARD_FETCHER_H
