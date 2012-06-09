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

#ifndef JABBER_CLIENT_INFO_SERVICE_H
#define JABBER_CLIENT_INFO_SERVICE_H

#include <QtCore/QWeakPointer>

#include "xmpp_discoitem.h"

class JabberProtocol;

namespace XMPP
{

class Client;
class Features;
class Status;

class JabberClientInfoService : public QObject
{
	Q_OBJECT

	QWeakPointer<Client> XmppClient;

	QString calculateCapsVersion() const;

public:
	explicit JabberClientInfoService(JabberProtocol *protocol);
	virtual ~JabberClientInfoService();

	void setClientName(const QString &clientName);
	void setClientVersion(const QString &clientVersion);
	void setOSName(const QString &osName);

	void setCapsNode(const QString &capsNode);
	void setFeatures(QStringList featureList);
	void setFeatures(const Features &features);

	void fillStatusCapsData(Status &status);

	void setIdentity(const DiscoItem::Identity &identity);

};

}

#endif // JABBER_CHAT_STATE_SERVICE_H
