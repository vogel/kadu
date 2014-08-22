/*
 * %kadu copyright begin%
 * Copyright 2010, 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2010, 2010, 2012 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2010, 2011, 2012, 2013 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2010, 2011, 2013 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#include "xmpp_tasks.h"

#include "buddies/buddy-manager.h"
#include "contacts/contact-manager.h"
#include "gui/windows/message-dialog.h"
#include "protocols/roster.h"
#include "debug.h"

#include "gui/windows/subscription-window.h"
#include "resource/jabber-resource-pool.h"
#include "jabber-contact-details.h"
#include "jabber-protocol.h"

#include "jabber-subscription-service.h"

namespace XMPP
{

JabberSubscriptionService::JabberSubscriptionService(JabberProtocol *protocol) :
		SubscriptionService(protocol), Protocol(protocol), XmppClient(protocol->xmppClient())
{
	connect(XmppClient.data(), SIGNAL(subscription(const Jid &, const QString &, const QString &)),
		   this, SLOT(subscription(const Jid &, const QString &, const QString &)));
}

void JabberSubscriptionService::subscription(const XMPP::Jid &jid, const QString &type, const QString &nick)
{
	Q_UNUSED(nick)

	if (type == "unsubscribed")
	{
		kdebug("%s revoked our presence authorization\n", jid.full().toUtf8().constData());
		/*
		 * Someone else removed our authorization to see them.
		 * We want to leave the contact in our contact list.
		 * In this case, we need to delete all the resources
		 * we have for it, as the Jabber server won't signal us
		 * that the contact is offline now.
		 */
		::Status offlineStatus;
		Contact contact = ContactManager::instance()->byId(Protocol->account(), jid.bare(), ActionReturnNull);

		if (contact)
		{
			::Status oldStatus = contact.currentStatus();
			contact.setCurrentStatus(offlineStatus);

			Protocol->emitContactStatusChanged(contact, oldStatus);
		}

		Protocol->resourcePool()->removeAllResources(jid);
	}

	if (type == "subscribe")
	{
		Contact contact = ContactManager::instance()->byId(Protocol->account(), jid.bare(), ActionCreate);
		SubscriptionWindow::getSubscription(contact, this, SLOT(authorizeContact(Contact, bool)));
	}
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
	sendSubsription(contact, "subscribed");
}

void JabberSubscriptionService::removeSubscription(const Contact &contact)
{
	sendSubsription(contact, "unsubscribed");
}

void JabberSubscriptionService::requestSubscription(const Contact &contact)
{
	sendSubsription(contact, "subscribe");
}

void JabberSubscriptionService::sendSubsription(const Contact &contact, const QString &subscription)
{
	if (!Protocol || !Protocol->isConnected() || contact.contactAccount() != Protocol->account() || !Protocol->xmppClient() || !Protocol->xmppClient()->isActive())
		return;

	XMPP::JT_Presence *task = new XMPP::JT_Presence(Protocol->xmppClient()->rootTask());
	task->sub(contact.id(), subscription);
	task->go(true);
}

}

#include "moc_jabber-subscription-service.cpp"
