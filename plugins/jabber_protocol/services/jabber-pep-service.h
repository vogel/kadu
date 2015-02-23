/* *
 * %kadu copyright begin%
 * Copyright 2013, 2014 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2012, 2013, 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * %kadu copyright end%
 *
 * Copyright (C) 2006 Remko Troncon
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

#ifndef JABBER_PEP_SERVICE_H
#define JABBER_PEP_SERVICE_H

#include <QtCore/QPointer>

class JabberProtocol;
class ServerInfoManager;

class PEPGetTask;

class JabberPepService : public QObject
{
	Q_OBJECT

	bool Enabled;

protected slots:
	// void messageReceived(const Message &message);
	// void publishFinished();

public:
	enum Access {
		DefaultAccess,
		PresenceAccess,
		PublicAccess
	};

	explicit JabberPepService(JabberProtocol *protocol);
	virtual ~JabberPepService();

//	Client * xmppClient() const;

	void setEnabled(bool enabled);
	bool enabled() const { return Enabled; }

	// void publish(const QString &node, const PubSubItem &item, Access = DefaultAccess);
	void retract(const QString &node, const QString &id);
	// PEPGetTask * get(const Jid &jid, const QString &node, const QString &id);

signals:
	// void publishSuccess(const QString &ns, const PubSubItem &item);
	// void publishError(const QString &ns, const PubSubItem &item);

	// void itemPublished(const Jid &jid, const QString &node, const PubSubItem &item);
	// void itemRetracted(const Jid &jid, const QString &node, const PubSubRetraction &retraction);

};

#endif // JABBER_PEP_SERVICE_H
