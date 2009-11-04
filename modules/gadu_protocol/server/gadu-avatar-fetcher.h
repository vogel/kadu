/***************************************************************************
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
*                                                                         *
***************************************************************************/

#ifndef GADU_AVATAR_FETCHER_H
#define GADU_AVATAR_FETCHER_H

#include <QtCore/QBuffer>
#include <QtGui/QPixmap>

#include "contacts/contact.h"

class QHttp;

class GaduAvatarFetcher : public QObject
{
	Q_OBJECT

	Contact *MyContactAccountData;
	QBuffer MyBuffer, AvatarBuffer;
	QHttp *MyHttp;
	
private slots:
	void requestFinished(int id, bool error);
	void avatarDownloaded(int id, bool error);

public:
	GaduAvatarFetcher(Contact *contactAccountData, QObject *parent = 0);
	void fetchAvatar();

signals:
	void avatarFetched(Contact *contactAccountData, const QByteArray &avatar);

};

#endif // GADU_AVATAR_FETCHER_H
