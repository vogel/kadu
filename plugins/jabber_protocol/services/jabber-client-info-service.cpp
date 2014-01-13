/*
 * %kadu copyright begin%
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2004 Adrian Smarzewski (adrian@kadu.net)
 * Copyright 2007, 2008, 2009, 2010, 2011, 2012 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2012, 2013 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#include <QtCore/QCryptographicHash>

#include <xmpp_client.h>

#include "jabber-protocol.h"

#include "jabber-client-info-service.h"

namespace XMPP
{

JabberClientInfoService::JabberClientInfoService(JabberProtocol *protocol) :
		QObject(protocol), XmppClient(protocol->xmppClient())
{
	setCapsNode("http://kadu.im/caps");

	DiscoItem::Identity identity;

	identity.category = "client";
	identity.type = "pc";
	identity.name = "Kadu";

	if (XmppClient)
		XmppClient->setIdentity(identity);
}

JabberClientInfoService::~JabberClientInfoService()
{
}

void JabberClientInfoService::setClientName(const QString &clientName)
{
	if (XmppClient)
		XmppClient->setClientName(clientName);
}

void JabberClientInfoService::setClientVersion(const QString &clientVersion)
{
	if (XmppClient)
		XmppClient->setClientVersion(clientVersion);
}

void JabberClientInfoService::setOSName(const QString &osName)
{
	if (XmppClient)
		XmppClient->setOSName(osName);
}

QString JabberClientInfoService::calculateCapsVersion() const
{
	if (!XmppClient)
		return QString();

	QString result(XmppClient->identity().category);
	result.append('/');
	result.append(XmppClient->identity().type);
	result.append("//");
	result.append(XmppClient->identity().name);
	result.append('<');
	result.append(XmppClient->features().list().join(QLatin1String("<")));
	result.append('<');

	return QString::fromAscii(QCryptographicHash::hash(result.toAscii(), QCryptographicHash::Sha1).toBase64());
}

void JabberClientInfoService::setCapsNode(const QString &capsNode)
{
	if (XmppClient)
	{
		XmppClient->setCapsNode(capsNode);
		XmppClient->setCapsVersion(calculateCapsVersion());
	}
}

void JabberClientInfoService::fillStatusCapsData(XMPP::Status &status)
{
	if (!XmppClient)
		return;

	status.setCapsNode(XmppClient->capsNode());
	status.setCapsVersion(XmppClient->capsVersion());
	status.setCapsHashAlgorithm(QLatin1String("sha-1"));
	status.setCapsExt(XmppClient->capsExt());
}

void JabberClientInfoService::setIdentity(const DiscoItem::Identity &identity)
{
	if (XmppClient)
		XmppClient->setIdentity(identity);
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
		XmppClient->setFeatures(features);
		XmppClient->setCapsVersion(calculateCapsVersion());
	}
}

}

#include "moc_jabber-client-info-service.cpp"
