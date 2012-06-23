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

#ifndef JABBER_AVATAR_PEP_FETCHER_H
#define JABBER_AVATAR_PEP_FETCHER_H

#include <QtCore/QWeakPointer>
#include <QtGui/QPixmap>

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
 * @class JabberAvatarPepFetcher
 * @short Class for feteching one avatar for Jabber/XMPP protocol.
 * @author Rafał 'Vogel' Malinowski
 *
 * This class allows for fetching one avatar for Jabber/XMPP protocol. To do that attach slot to avatarFetched()
 * signal and call fetchAvatar() method. After avatar is downloaded avatarFetched() signal is emitted and this
 * object deletes itself.
 *
 * Avatar is fetched using PEP protocol provyded by JabberPepService.
 *
 * If avatarFetched() is emitted with success flag and empty avatar then it is assumed that there is no avatar
 * for given user id.
 */
class JabberAvatarPepFetcher : public QObject
{
	Q_OBJECT

	QWeakPointer<JabberPepService> PepService;
	QString Id;
	QString AvatarId;

	void done(QPixmap avatar);
	void failed();

private slots:
	void discoItemsFinished();
	void avatarMetadataQueryFinished(const XMPP::Jid &jid, const QString &node, const XMPP::PubSubItem &item);
	void avatarDataQueryFinished(const XMPP::Jid &jid, const QString &node, const XMPP::PubSubItem &item);

public:
	/**
	 * @short Create new GaduAvatarFetcher instance.
	 * @author Rafał 'Vogel' Malinowski
	 * @param pepService pep service to use in this class
	 * @param parent QObject parent
	 */
	explicit JabberAvatarPepFetcher(JabberPepService *pepService, QObject *parent = 0);
	virtual ~JabberAvatarPepFetcher();

	/**
	 * @short Fetch avatar for contact with given id.
	 * @author Rafał 'Vogel' Malinowski
	 * @param id of contact to fetch avatar for
	 *
	 * Before calling this method attach to avatarFetched() signal to get informed about result. Please
	 * note that this method can be only called once. After that this object emits avatarFetched() and
	 * deletes itself.
	 */
	void fetchAvatar(const QString &id);

signals:
	/**
	 * @short Signal emitted when job of this class is done.
	 * @author Rafał 'Vogel' Malinowski
	 * @param ok success flag
	 * @param avatar downloaded avatar
	 *
	 * If ok is true then avatar is set to new avatar of given contact. If it is empty then contact does not
	 * have an avatar.
	 *
	 * If ok is false then operation failed and passed avatar is empty.
	 */
	void avatarFetched(bool ok, QPixmap avatar);

};

/**
 * @}
 */

#endif // JABBER_AVATAR_PEP_FETCHER_H
