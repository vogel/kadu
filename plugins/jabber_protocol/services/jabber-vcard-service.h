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

#ifndef JABBER_VCARD_SERVICE_H
#define JABBER_VCARD_SERVICE_H

#include <QtCore/QMap>
#include <QtCore/QWeakPointer>

namespace XMPP
{

class Client;
class Jid;
class JT_VCard;

class JabberVCardFetchCallback;

class JabberVCardService : public QObject
{
	Q_OBJECT

	QWeakPointer<XMPP::Client> XmppClient;
	QMap<JT_VCard *, JabberVCardFetchCallback *> Callbacks;

private slots:
	void fetched();

public:
	explicit JabberVCardService(QObject *parent = 0);
	virtual ~JabberVCardService();

	void setXmppClient(XMPP::Client *xmppClient);
	XMPP::Client * xmppClient() const;

	void fetch(const XMPP::Jid &jid, JabberVCardFetchCallback *callback);

};

}

#endif // JABBER_VCARD_SERVICE_H
