/*
 * %kadu copyright begin%
 * Copyright 2012 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2011, 2013 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#include "jabber-subscription-service.h"

#include "gui/windows/subscription-window.h"
#include "jabber-protocol.h"

#include "contacts/contact-manager.h"

#include <qxmpp/QXmppRosterManager.h>

JabberSubscriptionService::JabberSubscriptionService(QXmppRosterManager *roster, JabberProtocol *protocol) :
		SubscriptionService{protocol},
		m_roster{roster},
		m_protocol{protocol}
{
	connect(m_roster, SIGNAL(subscriptionReceived(QString)), this, SLOT(subscriptionReceived(QString)));
}

JabberSubscriptionService::~JabberSubscriptionService()
{
}

void JabberSubscriptionService::setContactManager(ContactManager *contactManager)
{
	m_contactManager = contactManager;
}

void JabberSubscriptionService::subscriptionReceived(const QString &bareJid)
{
	auto contact = m_contactManager->byId(m_protocol->account(), bareJid, ActionCreate);
	SubscriptionWindow::getSubscription(contact, this, SLOT(authorizeContact(Contact, bool)));
}

void JabberSubscriptionService::authorizeContact(Contact contact, bool authorized)
{
	if (authorized)
		resendSubscription(contact);
	else
		removeSubscription(contact);
}

void JabberSubscriptionService::resendSubscription(const Contact &contact)
{
	m_roster->acceptSubscription(contact.id());
}

void JabberSubscriptionService::removeSubscription(const Contact &contact)
{
	m_roster->refuseSubscription(contact.id());
}

void JabberSubscriptionService::requestSubscription(const Contact &contact)
{
	m_roster->subscribe(contact.id());
}

#include "moc_jabber-subscription-service.cpp"
