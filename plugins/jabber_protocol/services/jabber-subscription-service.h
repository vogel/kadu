/*
 * %kadu copyright begin%
 * Copyright 2008, 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2008 Michał Podsiadlik (michal@kadu.net)
 * Copyright 2007, 2008, 2009, 2010, 2011, 2012 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2007, 2008 Dawid Stawiarski (neeo@kadu.net)
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

#ifndef JABBER_SUBSCRIPTION_SERVICE_H
#define JABBER_SUBSCRIPTION_SERVICE_H

#include <QtCore/QObject>

#include <iris/xmpp_jid.h>

#include "protocols/services/subscription-service.h"

namespace XMPP
{

class Client;

class JabberProtocol;

class JabberSubscriptionService : public SubscriptionService
{
	Q_OBJECT

	JabberProtocol *Protocol;
	QPointer<Client> XmppClient;

private slots:
	void subscription(const Jid &jid, const QString &type, const QString &nick);

public:
	explicit JabberSubscriptionService(JabberProtocol *protocol);

	virtual void resendSubscription(const Contact &contact);
	virtual void removeSubscription(const Contact &contact);
	virtual void requestSubscription(const Contact &contact);

	void sendSubsription(const Contact &contact, const QString &subscription);

public slots:
	virtual void authorizeContact(Contact contact, bool authorized);

};

}

#endif // JABBER_SUBSCRIPTION_SERVICE_H
