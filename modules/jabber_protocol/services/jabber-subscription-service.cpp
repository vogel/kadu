/*
 * %kadu copyright begin%
 * Copyright 2010 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2010 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2010 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2010, 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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
#include "debug.h"

#include "gui/windows/subscription-window.h"
#include "jabber-contact-details.h"
#include "jabber-protocol.h"

#include "jabber-subscription-service.h"

JabberSubscriptionService::JabberSubscriptionService(JabberProtocol *protocol) :
		QObject(protocol), Protocol(protocol)
{
	connect(Protocol->client(), SIGNAL(subscription(const XMPP::Jid &, const QString &, const QString &)),
		   this, SLOT(subscription(const XMPP::Jid &, const QString &, const QString &)));
}

void JabberSubscriptionService::subscription(const XMPP::Jid &jid, const QString &type, const QString &nick)
{
	Q_UNUSED(nick)

	if (type == "unsubscribed")
	{
		/*
		 * Someone else removed our authorization to see them.
		 */
		kdebug("%s revoked our presence authorization", jid.full().toLocal8Bit().data());

		XMPP::JT_Roster *task;
		if (MessageDialog::ask("dialog-question", tr("Kadu"), tr("The user %1 removed subscription to you. "
					   "You will no longer be able to view his/her online/offline status. "
					   "Do you want to delete the contact?").arg(jid.full())))
		{
			/*
			 * Delete this contact from our roster.
			 */
			task = new XMPP::JT_Roster(Protocol->client()->rootTask());
			task->remove(jid);
			task->go(true);

			Contact contact = ContactManager::instance()->byId(Protocol->account(), jid.bare(), ActionReturnNull);
			BuddyManager::instance()->clearOwnerAndRemoveEmptyBuddy(contact);
		}
		else
		{
			/*
				 * We want to leave the contact in our contact list.
				 * In this case, we need to delete all the resources
				 * we have for it, as the Jabber server won't signal us
				 * that the contact is offline now.
			*/
			Status offlineStatus;
			offlineStatus.setType("Offline");
			Contact contact = ContactManager::instance()->byId(Protocol->account(), jid.bare(), ActionReturnNull);

			if (contact)
			{
				Status oldStatus = contact.currentStatus();
				contact.setCurrentStatus(offlineStatus);

				Protocol->emitContactStatusChanged(contact, oldStatus);
			}

			// Protocol->resourcePool()->removeAllResources(jid);
		}
	}

	if (type == "subscribe")
	{
		Contact contact = ContactManager::instance()->byId(Protocol->account(), jid.bare(), ActionCreate);
		if (contact.ownerBuddy().isAnonymous())
			SubscriptionWindow::getSubscription(contact, this, SLOT(authorizeContact(Contact, bool)));
		else
			authorizeContact(contact, MessageDialog::ask("dialog-question", tr("Kadu - authorize user?"), tr("The user %1 (%2) is asking for subscription from you. "
						   "He will be able to view your online/offline status. "
						   "Do you want to authorize the contact?").arg(contact.ownerBuddy().display(), jid.full())));
	}
}

void JabberSubscriptionService::authorizeContact(Contact contact, bool authorized)
{
	const XMPP::Jid jid = XMPP::Jid(contact.id());

	if (authorized)
		Protocol->client()->resendSubscription(jid);
	else
		Protocol->client()->rejectSubscription(jid);
}

void JabberSubscriptionService::resendSubscription(const Contact &contact)
{
	if (Protocol && Protocol->client())
		Protocol->client()->resendSubscription(contact.id());
}

void JabberSubscriptionService::removeSubscription(const Contact &contact)
{
	if (Protocol && Protocol->client())
		Protocol->client()->rejectSubscription(contact.id());
}

void JabberSubscriptionService::requestSubscription(const Contact &contact)
{
	if (Protocol && Protocol->client())
		Protocol->client()->requestSubscription(contact.id());
}
