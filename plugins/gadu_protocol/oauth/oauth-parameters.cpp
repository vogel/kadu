/*
 * %kadu copyright begin%
 * Copyright 2010, 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2010, 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2010, 2011, 2013 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#include <QtCore/QStringList>
#include <QtCore/QUrl>
#include <QtCrypto>

#include "oauth-parameters.h"

QString OAuthParameters::createUniqueNonce()
{
	return QCA::InitializationVector(16).toByteArray().toHex();
}

QString OAuthParameters::createTimestamp()
{
	return QString::number(QDateTime::currentDateTime().toTime_t());
}

OAuthParameters::OAuthParameters()
{
	setHttpMethod("POST");
	setNonce(createUniqueNonce());
	setTimestamp(createTimestamp());
	setSignatureMethod("HMAC-SHA1");
	setVerison("1.0");
}

OAuthParameters::OAuthParameters(const OAuthConsumer &consumer, const OAuthToken &token) :
		Consumer(consumer), Token(token)
{
	setHttpMethod("POST");
	setNonce(createUniqueNonce());
	setTimestamp(createTimestamp());
	setSignatureMethod("HMAC-SHA1");
	setVerison("1.0");
}

void OAuthParameters::setConsumer(const OAuthConsumer &consumer)
{
	Consumer = consumer;
}

const OAuthConsumer & OAuthParameters::consumer() const
{
	return Consumer;
}

void OAuthParameters::setHttpMethod(const QString &httpMethod)
{
	HttpMethod = httpMethod;
}

const QString & OAuthParameters::httpMethod() const
{
	return HttpMethod;
}

void OAuthParameters::setUrl(const QString &url)
{
	Url = url;
}

const QString & OAuthParameters::url() const
{
	return Url;
}

void OAuthParameters::setSignatureMethod(const QString &signatureMethod)
{
	SignatureMethod = signatureMethod;
}

const QString & OAuthParameters::signatureMethod() const
{
	return SignatureMethod;
}

void OAuthParameters::setNonce(const QString &nonce)
{
	Nonce = nonce;
}

const QString & OAuthParameters::nonce() const
{
	return Nonce;
}

void OAuthParameters::setTimestamp(const QString &timestamp)
{
	Timestamp = timestamp;
}

const QString & OAuthParameters::timestamp() const
{
	return Timestamp;
}

void OAuthParameters::setVerison(const QString &version)
{
	Version = version;
}

const QString & OAuthParameters::version() const
{
	return Version;
}

void OAuthParameters::setRealm(const QString &realm)
{
	Realm = realm;
}

const QString & OAuthParameters::realm() const
{
	return Realm;
}

void OAuthParameters::setSignature(const QByteArray &signature)
{
	Signature = signature;
}

const QByteArray & OAuthParameters::signature() const
{
	return Signature;
}


void OAuthParameters::setToken(const OAuthToken &token)
{
	Token = token;
}

const OAuthToken & OAuthParameters::token() const
{
	return Token;
}

void OAuthParameters::sign()
{
	QStringList baseItems;
	baseItems.append(HttpMethod);
	baseItems.append(Url.toUtf8().toPercentEncoding());
	baseItems.append(toSignatureBase());

	QByteArray key;
	key += Consumer.consumerSecret();
	key += '&';
	key += Token.tokenSecret();

	QCA::MessageAuthenticationCode hmac("hmac(sha1)", QCA::SymmetricKey(key));
	QCA::SecureArray array(baseItems.join("&").toUtf8());

	QByteArray digest = hmac.process(array).toByteArray().toBase64();
	setSignature(digest);
}

QByteArray OAuthParameters::toSignatureBase()
{
	QByteArray result;
	result += "oauth_consumer_key=";
	result += Consumer.consumerKey();
	result += '&';
	result += "oauth_nonce=";
	result += QUrl::toPercentEncoding(Nonce);
	result += '&';
	result +="oauth_signature_method=";
	result += QUrl::toPercentEncoding(SignatureMethod);
	result += '&';
	result += "oauth_timestamp=";
	result += QUrl::toPercentEncoding(Timestamp);
	result += '&';

	if (!Token.token().isEmpty())
	{
		result += "oauth_token=";
		result += Token.token();
		result += '&';
	}

	result += "oauth_version=";
	result += QUrl::toPercentEncoding(Version);

	return result.toPercentEncoding();
}

QByteArray OAuthParameters::toAuthorizationHeader()
{
	QByteArray result;
	result += "OAuth ";
	result += "realm=\"";
	result += QUrl::toPercentEncoding(Realm);
	result += "\", ";
	result += "oauth_nonce=\"";
	result += QUrl::toPercentEncoding(Nonce);
	result += "\", ";
	result += "oauth_timestamp=\"";
	result += QUrl::toPercentEncoding(Timestamp);
	result += "\", ";
	result += "oauth_consumer_key=\"";
	result += Consumer.consumerKey();
	result += "\", ";
	result += "oauth_signature_method=\"";
	result += QUrl::toPercentEncoding(SignatureMethod);
	result += "\", ";
	result += "oauth_version=\"";
	result += QUrl::toPercentEncoding(Version);
	result += "\", ";

	if (!Token.token().isEmpty())
	{
		result += "oauth_token=\"";
		result += Token.token();
		result += "\", ";
	}

	result += "oauth_signature=\"";
	result += Signature.toPercentEncoding();
	result += "\", ";

	return result;
}
