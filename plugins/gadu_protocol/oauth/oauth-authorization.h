/*
 * %kadu copyright begin%
 * Copyright 2008 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2008 Michał Podsiadlik (michal@kadu.net)
 * Copyright 2009 Bartłomiej Zimoń (uzi18@o2.pl)
 * Copyright 2004 Adrian Smarzewski (adrian@kadu.net)
 * Copyright 2007, 2008, 2009, 2010, 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#ifndef OAUTH_AUTHORIZATION_H
#define OAUTH_AUTHORIZATION_H

#include <QtCore/QObject>

#include "oauth/oauth-consumer.h"
#include "oauth/oauth-token.h"

class QNetworkAccessManager;
class QNetworkReply;

class OAuthAuthorization : public QObject
{
	Q_OBJECT

	OAuthToken Token;
	QString AuthorizationUrl;
	QString CallbackUrl;
	OAuthConsumer Consumer;
	QNetworkAccessManager *NetworkAccessManager;
	QNetworkReply *Reply;

private slots:
	void requestFinished();

public:
	explicit OAuthAuthorization(OAuthToken token, const QString &authorizationUrl, const QString &callbackUrl, OAuthConsumer consumer,
			QNetworkAccessManager *networkAccessManager, QObject *parent = 0);
	virtual ~OAuthAuthorization();

	void authorize();

signals:
	void authorized(bool ok);

};

#endif // OAUTH_AUTHORIZATION_H
