/*
 * %kadu copyright begin%
 * Copyright 2014 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2012, 2013, 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#pragma once

#include <QtCore/QPointer>
#include <QtGui/QPixmap>

#include "protocols/services/avatar-downloader.h"

class JabberVCardService;

class QXmppVCardIq;

/**
 * @addtogroup Jabber
 * @{
 */

/**
 * @class JabberAvatarDownloader
 * @short Class for downloading one avatar for Jabber/XMPP protocol using VCard.
 * @author Rafał 'Vogel' Malinowski
 *
 * This class allows for easy download of avatar for given contact from XMPP server. New instance can be created by
 * constructor that requires JabberVCardService argument.
 */
class JabberAvatarDownloader : public AvatarDownloader
{
	Q_OBJECT

	QPointer<JabberVCardService> VCardService;

	void done(QImage avatar);
	void failed();

private slots:
	void vCardDownloaded(bool ok, const QXmppVCardIq &vCard);

public:
	/**
	 * @short Create new JabberAvatarDownloader instance.
	 * @author Rafał 'Vogel' Malinowski
	 * @param vCardService vCard service to use in this class
	 * @param parent QObject parent
	 */
	explicit JabberAvatarDownloader(JabberVCardService *vCardService, QObject *parent = 0);
	virtual ~JabberAvatarDownloader();

	virtual void downloadAvatar(const QString &id);

};

/**
 * @}
 */
