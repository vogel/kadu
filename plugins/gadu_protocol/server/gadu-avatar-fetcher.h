/*
 * %kadu copyright begin%
 * Copyright 2009, 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2009, 2010, 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#ifndef GADU_AVATAR_FETCHER_H
#define GADU_AVATAR_FETCHER_H

#include <QtGui/QPixmap>

class QNetworkAccessManager;
class QNetworkReply;

/**
 * @addtogroup Gadu
 * @{
 */

/**
 * @class GaduAvatarFetcher
 * @short Class for feteching one avatar for Gadu Gadu protocol.
 * @author Rafał 'Vogel' Malinowski
 *
 * This class allows for fetching one avatar for Gadu Gadu protocol. To do that attach slot to avatarFetched()
 * signal and call fetchAvatar() method. After avatar is downloaded avatarFetched() signal is emitted and this
 * object deletes itself.
 *
 * Fetching avatars in Gadu Gadu protocol is just sending one HTTP request. No authorization is required. This
 * class supports up to 5 redirects from accessed url. After that it is assumed that avatar is not available
 * and avatarFetched() is emitted with failure flag.
 *
 * If avatarFetched() is emitted with success flag and empty avatar then it is assumed that there is no avatar
 * for given user id.
 */
class GaduAvatarFetcher : public QObject
{
	Q_OBJECT

	QNetworkAccessManager *NetworkAccessManager;
	QNetworkReply *Reply;
	int RedirectCount;

	void done(QPixmap avatar);
	void failed();

	void fetch(const QString &url);
	void parseReply();

private slots:
	void requestFinished();

public:
	/**
	 * @short Create new GaduAvatarFetcher instance.
	 * @author Rafał 'Vogel' Malinowski
	 * @param parent QObject parent
	 */
	explicit GaduAvatarFetcher(QObject *parent = 0);
	virtual ~GaduAvatarFetcher();

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

#endif // GADU_AVATAR_FETCHER_H
