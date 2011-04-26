/*
 * %kadu copyright begin%
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2009, 2010 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009, 2010, 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#ifndef JABBER_AVATAR_FETCHER_H
#define JABBER_AVATAR_FETCHER_H

#include <QtCore/QBuffer>
#include <QtGui/QPixmap>

#include "contacts/contact.h"

class QHttp;

class JabberAvatarFetcher : public QObject
{
	Q_OBJECT

	Contact MyContact;

	void fetchAvatarPEP();
	void fetchAvatarVCard();

private slots:
	void pepAvatarFetched(Contact contact, bool ok);
	void avatarFetchedSlot(Contact contact, bool ok);

public:
	JabberAvatarFetcher(Contact contact, QObject *parent );
	void fetchAvatar();

signals:
	void avatarFetched(Contact contact, bool ok);

};

#endif // JABBER_AVATAR_FETCHER_H
