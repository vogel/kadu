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
#include <QtCore/QFile>
#include <QtGui/QImage>
#include <QtNetwork/QHttp>
#include <QtCore/QUrl>

#include "contacts/contact-account-data.h"

class GaduAvatarFetcher : public QObject
{
	Q_OBJECT

	ContactAccountData *cad;
	QBuffer buff;
	QFile *file;
	QHttp *h;


public:
	GaduAvatarFetcher(QObject *parent = 0) : QObject(parent) {}
	void fetchAvatar(ContactAccountData *contactAccountData);
private slots:
	void requestFinished(int id, bool error);
	void avatarDownloaded(int id, bool error);
signals:
	void avatarFetched(ContactAccountData *contactAccountData, QPixmap avatar);
};

#endif // GADU_AVATAR_FETCHER_H
