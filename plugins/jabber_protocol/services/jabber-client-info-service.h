/*
 * %kadu copyright begin%
 * Copyright 2012, 2013 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2012, 2014 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#include <QtCore/QPointer>

#include "xmpp_discoitem.h"

class JabberProtocol;

namespace XMPP
{

class Client;
class Features;
class Status;

/**
 * @addtogroup Jabber
 * @{
 */

/**
 * @class JabberClientInfoService
 * @author Rafał 'Vogel' Malinowski
 * @short Service that allows setting various client info data for XMPP connection.
 *
 * This service allows setting various client info data for XMPP connection, like client name and
 * version, OS name, capabilities and features and identity.
 *
 * All this data can be requested at any time by server or other clients.
 *
 * When parent JabberProtocol does not have valid XMPP::Client object this service will do nothing.
 *
 * By default Identity is set to {client, pc, Kadu} and Caps Node to http://kadu.im/caps.
 */
class JabberClientInfoService : public QObject
{
	Q_OBJECT

	QPointer<Client> XmppClient;

	QString calculateCapsVersion() const;

public:
	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Create new instance of JabberClientInfoService.
	 * @param protocol parent protocol handler
	 */
	explicit JabberClientInfoService(JabberProtocol *protocol);
	virtual ~JabberClientInfoService();

	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Set value of Client Name property of connection.
	 * @param clientName new value of Client Name property of connection
	 */
	void setClientName(const QString &clientName);

	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Set value of Client Version property of connection.
	 * @param clientName new value of Client Version property of connection
	 */
	void setClientVersion(const QString &clientVersion);

	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Set value of OS Name property of connection.
	 * @param clientName new value of OS Name property of connection
	 */
	void setOSName(const QString &osName);

	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Set value of Caps Node property of connection.
	 * @param clientName new value of Caps Node property of connection
	 *
	 * Value of Caps Version property is recalculated and set for connection.
	 */
	void setCapsNode(const QString &capsNode);

	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Sets list of supported features.
	 * @param featureList list of supported features
	 *
	 * List is sorted before use.
	 * Value of Caps Version property is recalculated and set for connection.
	 */
	void setFeatures(QStringList featureList);

	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Sets supported features.
	 * @param features supported features
	 *
	 * Value of Caps Version property is recalculated and set for connection.
	 */
	void setFeatures(const Features &features);

	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Fills Status object with capabilities data.
	 * @param status status to fill data with
	 */
	void fillStatusCapsData(Status &status);

	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Set value of Identity property of connection.
	 * @param clientName new value of Identity property of connection
	 */
	void setIdentity(const DiscoItem::Identity &identity);

};

/**
 * @}
 */

}

#endif // JABBER_CHAT_STATE_SERVICE_H
