/*
 * %kadu copyright begin%
 * Copyright 2011 Tomasz Rostanski (rozteck@interia.pl)
 * Copyright 2008, 2010 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2008 Michał Podsiadlik (michal@kadu.net)
 * Copyright 2009 Bartłomiej Zimoń (uzi18@o2.pl)
 * Copyright 2004 Adrian Smarzewski (adrian@kadu.net)
 * Copyright 2007, 2008, 2009, 2010, 2011, 2012, 2013 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2011, 2014 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#ifndef JABBER_AVATAR_VCARD_DOWNLOADER_H
#define JABBER_AVATAR_VCARD_DOWNLOADER_H

#include <QtGui/QPixmap>

#include <xmpp_vcard.h>

#include "protocols/services/avatar-downloader.h"

namespace XMPP
{
	class JabberVCardService;
}

/**
 * @addtogroup Jabber
 * @{
 */

/**
 * @class JabberAvatarVCardDownloader
 * @short Class for downloading one avatar for Jabber/XMPP protocol using VCard.
 * @author Rafał 'Vogel' Malinowski
 *
 * This class allows for easy download of avatar for given contact from XMPP server. New instance can be created by
 * constructor that requires XMPP::JabberVCardService argument.
 */
class JabberAvatarVCardDownloader : public AvatarDownloader
{
	Q_OBJECT

	QPointer<XMPP::JabberVCardService> VCardService;

	void done(QImage avatar);
	void failed();

private slots:
	void vCardDownloaded(bool ok, XMPP::VCard vCard);

public:
	/**
	 * @short Create new JabberAvatarVCardDownloader instance.
	 * @author Rafał 'Vogel' Malinowski
	 * @param vCardService vCard service to use in this class
	 * @param parent QObject parent
	 */
	explicit JabberAvatarVCardDownloader(XMPP::JabberVCardService *vCardService, QObject *parent = 0);
	virtual ~JabberAvatarVCardDownloader();

	virtual void downloadAvatar(const QString &id);

};

/**
 * @}
 */

#endif // JABBER_AVATAR_VCARD_DOWNLOADER_H
