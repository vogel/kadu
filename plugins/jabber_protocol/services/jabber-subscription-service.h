/*
 * %kadu copyright begin%
 * Copyright 2014 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2011, 2012, 2013, 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#pragma once

#include "protocols/services/subscription-service.h"

#include <QtCore/QObject>
#include <QtCore/QPointer>

class JabberProtocol;

class ContactManager;

class QXmppRosterManager;

class JabberSubscriptionService : public SubscriptionService
{
	Q_OBJECT

public:
	explicit JabberSubscriptionService(QXmppRosterManager *roster, JabberProtocol *protocol);
	virtual ~JabberSubscriptionService();

	void setContactManager(ContactManager *contactManager);

	virtual void resendSubscription(const Contact &contact);
	virtual void removeSubscription(const Contact &contact);
	virtual void requestSubscription(const Contact &contact);

public slots:
	virtual void authorizeContact(Contact contact, bool authorized);

private:
	QPointer<QXmppRosterManager> m_roster;
	JabberProtocol *m_protocol;

	QPointer<ContactManager> m_contactManager;

private slots:
	void subscriptionReceived(const QString &bareJid);

};
