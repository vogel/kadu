/*
 * %kadu copyright begin%
 * Copyright 2012 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include <QtCore/QCryptographicHash>

#include "jabber-protocol.h"

#include "jabber-client-info-service.h"

using namespace XMPP;

JabberClientInfoService::JabberClientInfoService(JabberProtocol *protocol) :
		QObject(protocol), XmppClient(protocol->xmppClient())
{
	setCapsNode("http://kadu.im/caps");

	DiscoItem::Identity identity;

	identity.category = "client";
	identity.name = "pc";
	identity.type = "Kadu";

	if (XmppClient)
		XmppClient.data()->setIdentity(identity);
}

JabberClientInfoService::~JabberClientInfoService()
{
}

void JabberClientInfoService::setClientName(const QString &clientName)
{
	if (XmppClient)
		XmppClient.data()->setClientName(clientName);
}

void JabberClientInfoService::setClientVersion(const QString &clientVersion)
{
	if (XmppClient)
		XmppClient.data()->setClientVersion(clientVersion);
}

void JabberClientInfoService::setOSName(const QString &osName)
{
	if (XmppClient)
		XmppClient.data()->setOSName(osName);
}

QString JabberClientInfoService::calculateCapsVersion() const
{
	if (!XmppClient)
		return QString();

	QString result(XmppClient.data()->identity().category);
	result.append('/');
	result.append(XmppClient.data()->identity().type);
	result.append("//");
	result.append(XmppClient.data()->identity().name);
	result.append('<');
	result.append(XmppClient.data()->features().list().join(QLatin1String("<")));
	result.append('<');

	return QString::fromAscii(QCryptographicHash::hash(result.toAscii(), QCryptographicHash::Sha1).toBase64());
}

void JabberClientInfoService::setCapsNode(const QString &capsNode)
{
	if (XmppClient)
	{
		XmppClient.data()->setCapsNode(capsNode);
		XmppClient.data()->setCapsVersion(calculateCapsVersion());
	}
}

void JabberClientInfoService::fillStatusCapsData(XMPP::Status &status)
{
	if (!XmppClient)
		return;

	status.setCapsNode(XmppClient.data()->capsNode());
	status.setCapsVersion(XmppClient.data()->capsVersion());
	status.setCapsHashAlgorithm(QLatin1String("sha-1"));
	status.setCapsExt(XmppClient.data()->capsExt());
}

void JabberClientInfoService::setIdentity(const DiscoItem::Identity &identity)
{
	if (XmppClient)
		XmppClient.data()->setIdentity(identity);
}

void JabberClientInfoService::setFeatures(QStringList featureList)
{
	featureList.sort();
	Features features(featureList);
	setFeatures(features);
}

void JabberClientInfoService::setFeatures(const Features &features)
{
	if (XmppClient)
	{
		XmppClient.data()->setFeatures(features);
		XmppClient.data()->setCapsVersion(calculateCapsVersion());
	}
}
