/*
 * %kadu copyright begin%
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2009, 2010 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#ifndef GADU_AVATAR_FETCHER_H
#define GADU_AVATAR_FETCHER_H

#include <QtCore/QBuffer>
#include <QtGui/QPixmap>

#include "contacts/contact.h"

class QHttp;

class GaduAvatarFetcher : public QObject
{
	Q_OBJECT

	Contact MyContact;
	QBuffer MyBuffer, AvatarBuffer;
	QHttp *MyHttp;

	void done();
	void failed();

private slots:
	void requestFinished(int id, bool error);
	void avatarDownloaded(int id, bool error);

public:
	explicit GaduAvatarFetcher(Contact contact, QObject *parent = 0);
	void fetchAvatar();

signals:
	void avatarFetched(Contact contact, bool ok);

};

#endif // GADU_AVATAR_FETCHER_H
