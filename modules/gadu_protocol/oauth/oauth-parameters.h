/*
 * %kadu copyright begin%
 * Copyright 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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
#include <QtCrypto>

#include "oauth/oauth-token.h"

class OAuthParameters
{
	QCA::Initializer QCAInit;

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
	OAuthParameters(OAuthConsumer consumer, OAuthToken token);

	void setConsumer(OAuthConsumer consumer);
	OAuthConsumer consumer();

	void setHttpMethod(const QString &httpMethod);
	QString httpMethod();

	void setUrl(const QString &url);
	QString url();
	
	void setSignatureMethod(const QString &signatureMethod);
	QString signatureMethod();
	
	void setNonce(const QString &nonce);
	QString nonce();
	
	void setTimestamp(const QString &timestamp);
	QString timestamp();
	
	void setVerison(const QString &version);
	QString version();
	
	void setRealm(const QString &realm);
	QString realm();

	void setSignature(const QByteArray &signature);
	QByteArray signature();

	void setToken(const OAuthToken &token);
	OAuthToken token();

	void sign();

	QByteArray toSignatureBase();
	QByteArray toAuthorizationHeader();

};

#endif // OAUTH_PARAMETERS_H
