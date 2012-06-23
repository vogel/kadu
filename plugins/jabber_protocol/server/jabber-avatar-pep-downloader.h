/*
 * %kadu copyright begin%
 * Copyright 2011 Tomasz Rostanski (rozteck@interia.pl)
 * Copyright 2008, 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
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

#ifndef JABBER_AVATAR_PEP_DOWNLOADER_H
#define JABBER_AVATAR_PEP_DOWNLOADER_H

#include <QtCore/QWeakPointer>

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
 * @short Class for feteching one avatar for Jabber/XMPP protocol using PEP.
 * @author Rafał 'Vogel' Malinowski
 */
class JabberAvatarPepDownloader : public AvatarDownloader
{
	Q_OBJECT

	QWeakPointer<JabberPepService> PepService;
	QString Id;
	QString AvatarId;

	void done(QImage avatar);
	void failed();

private slots:
	void discoItemsFinished();
	void avatarMetadataQueryFinished(const XMPP::Jid &jid, const QString &node, const XMPP::PubSubItem &item);
	void avatarDataQueryFinished(const XMPP::Jid &jid, const QString &node, const XMPP::PubSubItem &item);

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
