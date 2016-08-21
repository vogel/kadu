/*
 * %kadu copyright begin%
 * Copyright 2011, 2012, 2013 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#include "accounts/account.h"
#include "buddies/buddy-set.h"
#include "contacts/contact.h"
#include "core/myself.h"
#include "gui/actions/action-context.h"
#include "gui/actions/action-description.h"
#include "gui/actions/action.h"
#include "gui/actions/actions.h"
#include "plugin/plugin-injected-factory.h"
#include "protocols/protocol.h"

#include "actions/show-xml-console-action-description.h"
#include "jabber-protocol.h"

#include "jabber-actions.h"

static void disableNoRosterContact(Myself *myself, Action *action)
{
	action->setEnabled(false);

	if (action->context()->buddies().isAnyTemporary())
		return;

	const Contact &contact = action->context()->contacts().toContact();
	if (!contact)
		return;

	if (action->context()->buddies().contains(myself->buddy()))
		return;

	Account account = contact.contactAccount();
	if (!account || !account.protocolHandler() || !account.protocolHandler()->rosterService())
		return;

	if (!account.protocolHandler()->isConnected())
		return;

	action->setEnabled(true);
}

JabberActions::JabberActions(QObject *parent) :
		QObject{parent}
{
}

JabberActions::~JabberActions()
{
}

void JabberActions::setActions(Actions *actions)
{
	m_actions = actions;
}

void JabberActions::setPluginInjectedFactory(PluginInjectedFactory *pluginInjectedFactory)
{
	m_pluginInjectedFactory = pluginInjectedFactory;
}

void JabberActions::setMyself(Myself *myself)
{
	m_myself = myself;
}

void JabberActions::init()
{
	auto showXmlConsoleAction = m_pluginInjectedFactory->makeInjected<ShowXmlConsoleActionDescription>(this);
	m_actions->insert(showXmlConsoleAction);

	m_actions->blockSignals();

	ResendSubscription = m_pluginInjectedFactory->makeInjected<ActionDescription>(this, ActionDescription::TypeUser, "rosterResendSubscription",
			this, SLOT(resendSubscriptionActionActivated(QAction*)), KaduIcon(), tr("Resend Subscription"),
			false,
			[this](Action *action){ return disableNoRosterContact(m_myself, action); });
	RemoveSubscription = m_pluginInjectedFactory->makeInjected<ActionDescription>(this, ActionDescription::TypeUser, "rosterRemoveSubscription",
			this, SLOT(removeSubscriptionActionActivated(QAction*)), KaduIcon(), tr("Remove Subscription"),
			false,
			[this](Action *action){ return disableNoRosterContact(m_myself, action); });

	// The last ActionDescription will send actionLoaded() signal.
	m_actions->unblockSignals();

	AskForSubscription = m_pluginInjectedFactory->makeInjected<ActionDescription>(this, ActionDescription::TypeUser, "rosterAskForSubscription",
			this, SLOT(askForSubscriptionActionActivated(QAction*)), KaduIcon(), tr("Ask for Subscription"),
			false,
			[this](Action *action){ return disableNoRosterContact(m_myself, action); });
}

Contact JabberActions::contactFromAction(QAction *action)
{
	Action *kaduAction = qobject_cast<Action *>(action);
	if (!kaduAction)
		return Contact::null;

	return kaduAction->context()->contacts().toContact();
}

SubscriptionService * JabberActions::subscriptionServiceFromContact(const Contact &contact)
{
	JabberProtocol *jabberProtocolHandler = qobject_cast<JabberProtocol *>(contact.contactAccount().protocolHandler());
	if (!jabberProtocolHandler)
		return 0;

	return jabberProtocolHandler->subscriptionService();
}

void JabberActions::resendSubscriptionActionActivated(QAction *sender)
{
	Contact contact = contactFromAction(sender);
	if (!contact)
		return;

	SubscriptionService *subscriptionService = subscriptionServiceFromContact(contact);
	if (!subscriptionService)
		return;

	subscriptionService->resendSubscription(contact);
}

void JabberActions::removeSubscriptionActionActivated(QAction *sender)
{
	Contact contact = contactFromAction(sender);
	if (!contact)
		return;

	SubscriptionService *subscriptionService = subscriptionServiceFromContact(contact);
	if (!subscriptionService)
		return;

	subscriptionService->removeSubscription(contact);
}

void JabberActions::askForSubscriptionActionActivated(QAction *sender)
{
	Contact contact = contactFromAction(sender);
	if (!contact)
		return;

	SubscriptionService *subscriptionService = subscriptionServiceFromContact(contact);
	if (!subscriptionService)
		return;

	subscriptionService->requestSubscription(contact);
}

#include "moc_jabber-actions.cpp"
