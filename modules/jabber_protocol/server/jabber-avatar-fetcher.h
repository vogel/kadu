/***************************************************************************
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
*                                                                         *
***************************************************************************/

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
	
private slots:
	void receivedVCard();

public:
	JabberAvatarFetcher(Contact contact, QObject *parent = 0);
	void fetchAvatar();

signals:
	void avatarFetched(Contact contact, const QByteArray &avatar);

};

#endif // JABBER_AVATAR_FETCHER_H
