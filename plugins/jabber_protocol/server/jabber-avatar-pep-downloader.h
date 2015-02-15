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

#ifndef JABBER_AVATAR_PEP_DOWNLOADER_H
#define JABBER_AVATAR_PEP_DOWNLOADER_H

#include <QtCore/QPointer>

#include "protocols/services/avatar-downloader.h"

namespace XMPP
{
	class Jid;
	class PubSubItem;
}

class JabberPepService;

/**
 * @addtogroup Jabber
 * @{
 */

/**
 * @class JabberAvatarPepDownloader
 * @short Class for downloading one avatar for Jabber/XMPP protocol using PEP.
 * @author Rafał 'Vogel' Malinowski
 *
 * This class allows for easy download of avatar for given contact from XMPP server. New instance can be created by
 * constructor that requires JabberPepService argument.
 */
class JabberAvatarPepDownloader : public AvatarDownloader
{
	Q_OBJECT

	QPointer<JabberPepService> PepService;
	QString Id;
	QString AvatarId;

	void done(QImage avatar);
	void failed();

private slots:
	void discoItemsFinished();
	void avatarMetadataQueryFinished();
	void avatarDataQueryFinished();

public:
	/**
	 * @short Create new JabberAvatarPepDownloader instance.
	 * @author Rafał 'Vogel' Malinowski
	 * @param pepService pep service to use in this class
	 * @param parent QObject parent
	 */
	explicit JabberAvatarPepDownloader(JabberPepService *pepService, QObject *parent = 0);
	virtual ~JabberAvatarPepDownloader();

	virtual void downloadAvatar(const QString &id);

};

/**
 * @}
 */

#endif // JABBER_AVATAR_PEP_DOWNLOADER_H
