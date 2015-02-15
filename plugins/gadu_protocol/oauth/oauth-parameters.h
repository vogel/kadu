/*
 * %kadu copyright begin%
 * Copyright 2011 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2011 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#ifndef OAUTH_PARAMETERS_H
#define OAUTH_PARAMETERS_H

#include <QtCore/QString>

#include "oauth/oauth-token.h"

class OAuthParameters
{
	OAuthConsumer Consumer;
	QString HttpMethod;
	QString Url;
	QString SignatureMethod;
	QString Nonce;
	QString Timestamp;
	QString Version;
	QString Realm;
	QByteArray Signature;
	OAuthToken Token;

public:
	static QString createUniqueNonce();
	static QString createTimestamp();

	OAuthParameters();
	OAuthParameters(const OAuthConsumer &consumer, const OAuthToken &token);

	void setConsumer(const OAuthConsumer &consumer);
	const OAuthConsumer & consumer() const;

	void setHttpMethod(const QString &httpMethod);
	const QString & httpMethod() const;

	void setUrl(const QString &url);
	const QString & url() const;

	void setSignatureMethod(const QString &signatureMethod);
	const QString & signatureMethod() const;

	void setNonce(const QString &nonce);
	const QString & nonce() const;

	void setTimestamp(const QString &timestamp);
	const QString & timestamp() const;

	void setVerison(const QString &version);
	const QString & version() const;

	void setRealm(const QString &realm);
	const QString & realm() const;

	void setSignature(const QByteArray &signature);
	const QByteArray & signature() const;

	void setToken(const OAuthToken &token);
	const OAuthToken & token() const;

	void sign();

	QByteArray toSignatureBase();
	QByteArray toAuthorizationHeader();

};

#endif // OAUTH_PARAMETERS_H
