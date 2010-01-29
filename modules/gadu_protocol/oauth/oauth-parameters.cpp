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

#include "oauth-parameters.h"

OAuthParameters::OAuthParameters()
{
}

void OAuthParameters::setConsumerKey(const QString &consumerKey)
{
	ConsumerKey = consumerKey;
}

QString OAuthParameters::consumerKey()
{
	return ConsumerKey;
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

void OAuthParameters::setSignature(const QString &signature)
{
	Signature = signature;
}

QString OAuthParameters::signature()
{
	return Signature;
}

QByteArray OAuthParameters::toSignatureBase()
{
	QStringList result;
	result.append(QString("oauth_consumer_key=%1").arg(ConsumerKey));
	result.append(QString("oauth_nonce=%1").arg(Nonce));
	result.append(QString("oauth_signature_method=%1").arg(SignatureMethod));
	result.append(QString("oauth_timestamp=%1").arg(Timestamp));
	result.append(QString("oauth_version=%1").arg(Version));

	return result.join("&").toLocal8Bit().toPercentEncoding();
}

QString OAuthParameters::toAuthorizationHeader()
{
	QStringList result;
	result.append(QString("realm=\"%1\"").arg(Realm));
	result.append(QString("oauth_nonce=\"%1\"").arg(Nonce));
	result.append(QString("oauth_timestamp=\"%1\"").arg(Timestamp));
	result.append(QString("oauth_consumer_key=\"%1\"").arg(ConsumerKey));
	result.append(QString("oauth_signature_method=\"%1\"").arg(SignatureMethod));
	result.append(QString("oauth_version=\"%1\"").arg(Version));
	result.append(QString("oauth_signature=\"%1\"").arg(QString(Signature.toLocal8Bit().toPercentEncoding())));

	return QString("OAuth %1").arg(result.join(", "));
}
