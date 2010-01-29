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

#include "oauth/oauth-token.h"

class OAuthParameters
{
	QString ConsumerKey;
	QString SignatureMethod;
	QString Nonce;
	QString Timestamp;
	QString Version;
	QString Realm;
	QString Signature;
	OAuthToken Token;

public:
	OAuthParameters();

	void setConsumerKey(const QString &consumerKey);
	QString consumerKey();
	
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

	void setSignature(const QString &signature);
	QString signature();

	void setToken(const OAuthToken &token);
	OAuthToken token();

	QByteArray toSignatureBase();
	QString toAuthorizationHeader();

};

#endif // OAUTH_PARAMETERS_H
