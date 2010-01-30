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

#include <QtCore/QStringList>
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

OAuthParameters::OAuthParameters(OAuthConsumer consumer, OAuthToken token) :
		Consumer(consumer), Token(token)
{
	setHttpMethod("POST");
	setNonce(createUniqueNonce());
	setTimestamp(createTimestamp());
	setSignatureMethod("HMAC-SHA1");
	setVerison("1.0");
}

void OAuthParameters::setConsumer(OAuthConsumer consumer)
{
	Consumer = consumer;
}

OAuthConsumer OAuthParameters::consumer()
{
	return Consumer;
}

void OAuthParameters::setHttpMethod(const QString &httpMethod)
{
	HttpMethod = httpMethod;
}

QString OAuthParameters::httpMethod()
{
	return HttpMethod;
}

void OAuthParameters::setUrl(const QString &url)
{
	Url = url;
}

QString OAuthParameters::url()
{
	return Url;
}

void OAuthParameters::setSignatureMethod(const QString &signatureMethod)
{
	SignatureMethod = signatureMethod;
}

QString OAuthParameters::signatureMethod()
{
	return SignatureMethod;
}

void OAuthParameters::setNonce(const QString &nonce)
{
	Nonce = nonce;
}

QString OAuthParameters::nonce()
{
	return Nonce;
}

void OAuthParameters::setTimestamp(const QString &timestamp)
{
	Timestamp = timestamp;
}

QString OAuthParameters::timestamp()
{
	return Timestamp;
}

void OAuthParameters::setVerison(const QString &version)
{
	Version = version;
}

QString OAuthParameters::version()
{
	return Version;
}

void OAuthParameters::setRealm(const QString &realm)
{
	Realm = realm;
}

QString OAuthParameters::realm()
{
	return Realm;
}

void OAuthParameters::setSignature(const QByteArray &signature)
{
	Signature = signature;
}

QByteArray OAuthParameters::signature()
{
	return Signature;
}


void OAuthParameters::setToken(const OAuthToken &token)
{
	Token = token;
}

OAuthToken OAuthParameters::token()
{
	return Token;
}

void OAuthParameters::sign()
{
	QStringList baseItems;
	baseItems.append(HttpMethod);
	baseItems.append(Url.toLocal8Bit().toPercentEncoding());
	baseItems.append(toSignatureBase());

	QByteArray key;
	key += Consumer.consumerSecret();
	key += "&";
	key += Token.tokenSecret();

	QCA::MessageAuthenticationCode hmac("hmac(sha1)", QCA::SymmetricKey(key));
	QCA::SecureArray array(baseItems.join("&").toLocal8Bit());
	hmac.update(array);

	QByteArray digest = hmac.final().toByteArray().toBase64();
	setSignature(digest);
}

QByteArray OAuthParameters::toSignatureBase()
{
	QByteArray result;
	result += "oauth_consumer_key=";
	result += Consumer.consumerKey();
	result += "&";
	result += "oauth_nonce=";
	result += Nonce;
	result += "&";
	result +="oauth_signature_method=";
	result += SignatureMethod;
	result += "&";
	result += "oauth_timestamp=";
	result += Timestamp;
	result += "&";

	if (!Token.token().isEmpty())
	{
		result += "oauth_token=";
		result += Token.token();
		result += "&";
	}

	result += "oauth_version=";
	result += Version;

	return result.toPercentEncoding();
}

QByteArray OAuthParameters::toAuthorizationHeader()
{
	QByteArray result;
	result += "OAuth ";
	result += "realm=\"";
	result += Realm;
	result += "\", ";
	result += "oauth_nonce=\"";
	result += Nonce;
	result += "\", ";
	result += "oauth_timestamp=\"";
	result += Timestamp;
	result += "\", ";
	result += "oauth_consumer_key=\"";
	result += Consumer.consumerKey();
	result += "\", ";
	result += "oauth_signature_method=\"";
	result += SignatureMethod;
	result += "\", ";
	result += "oauth_version=\"";
	result += Version;
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
