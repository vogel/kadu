/*
 * %kadu copyright begin%
 * Copyright 2009, 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2009, 2010, 2011, 2012 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2011 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#ifndef JABBER_AVATAR_DOWNLOADER_H
#define JABBER_AVATAR_DOWNLOADER_H

#include <QtCore/QPointer>

#include "protocols/services/avatar-downloader.h"

namespace XMPP { class JabberVCardService; }

class JabberPepService;

/**
 * @addtogroup Jabber
 * @{
 */

/**
 * @class JabberAvatarDownloader
 * @short Downloads avatar from XMPP server using PEP or VCard.
 * @author Rafał 'Vogel' Malinowski
 *
 * This class allows for easy download of avatar from XMPP server. New instance can be created by constructor that requires
 * JabberPepService and XMPP::JabberVCardService arguments. If both services are null then this class will always fail
 * to do it job. If one is provided then it will be used to download avatar. If both are provided then PEP service will
 * be used and VCard only when PEP service is not enabled or when it failed.
 *
 * This class internally used JabberAvatarPepDownloader and JabberAvatarVCardDownloader.
 */
class JabberAvatarDownloader : public AvatarDownloader
{
	Q_OBJECT

	QString Id;
	QPointer<JabberPepService> PepService;
	QPointer<XMPP::JabberVCardService> VCardService;

	void failed();

	void downloadAvatarPEP();
	void downloadAvatarVCard();

private slots:
	void pepAvatarDownloaded(bool ok, QImage avatar);
	void vCardAvatarDownloaded(bool ok, QImage avatar);

public:
	/**
	 * @short Create new JabberAvatarPepDownloader instance.
	 * @author Rafał 'Vogel' Malinowski
	 * @param pepService pep service to use in this class
	 * @param vCardService vCard service to use in this class
	 * @param parent QObject parent
	 */
	explicit JabberAvatarDownloader(JabberPepService *pepService, XMPP::JabberVCardService *vCardService, QObject *parent);
	virtual ~JabberAvatarDownloader();

	virtual void downloadAvatar(const QString &id);

};

/**
 * @}
 */

#endif // JABBER_AVATAR_DOWNLOADER_H
