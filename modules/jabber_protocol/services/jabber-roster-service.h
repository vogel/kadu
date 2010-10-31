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

#ifndef JABBER_ROSTER_SERVICE_H
#define JABBER_ROSTER_SERVICE_H

#include <QtCore/QObject>


namespace XMPP
{
	class RosterItem;
};

class Buddy;
class Contact;
class JabberProtocol;

class JabberRosterService : public QObject
{
	Q_OBJECT

	JabberProtocol *Protocol;

	QList<Contact> ContactsForDelete;
	bool InRequest;

	const QString & itemDisplay(const XMPP::RosterItem &item);
	Buddy itemBuddy(const XMPP::RosterItem &item, const Contact &contact);

private slots:
	void contactUpdated(const XMPP::RosterItem &item);
	void contactDeleted(const XMPP::RosterItem &item);
	void rosterRequestFinished(bool success);

public:
	explicit JabberRosterService(JabberProtocol *protocol);
	virtual ~JabberRosterService();

	void downloadRoster();

signals:
	void rosterDownloaded(bool success);

};

#endif // JABBER_ROSTER_SERVICE_H
