/*
 * %kadu copyright begin%
 * Copyright 2016 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "subscription-action.h"

#include "jabber-protocol.h"

#include "actions/action-context.h"
#include "actions/action.h"
#include "contacts/contact-set.h"
#include "contacts/contact.h"
#include "core/myself.h"

SubscriptionAction::SubscriptionAction(QObject *parent) : ActionDescription(parent)
{
    setType(ActionDescription::TypeUser);
    setName("rosterResendSubscription");
    setText(tr("Resend Subscription"));
}

SubscriptionAction::~SubscriptionAction()
{
}

void SubscriptionAction::setMyself(Myself *myself)
{
    m_myself = myself;
}

Contact SubscriptionAction::contactFromAction(QAction *action)
{
    auto kaduAction = qobject_cast<Action *>(action);
    if (!kaduAction)
        return Contact::null;

    return kaduAction->context()->contacts().toContact();
}

SubscriptionService *SubscriptionAction::subscriptionServiceFromContact(const Contact &contact)
{
    auto jabberProtocolHandler = qobject_cast<JabberProtocol *>(contact.contactAccount().protocolHandler());
    if (!jabberProtocolHandler)
        return 0;

    return jabberProtocolHandler->subscriptionService();
}

void SubscriptionAction::actionTriggered(QAction *sender, bool)
{
    auto contact = contactFromAction(sender);
    if (!contact)
        return;

    auto subscriptionService = subscriptionServiceFromContact(contact);
    if (!subscriptionService)
        return;

    execute(subscriptionService, contact);
}

void SubscriptionAction::updateActionState(Action *action)
{
    action->setEnabled(false);

    if (action->context()->buddies().isAnyTemporary())
        return;

    auto const &contact = action->context()->contacts().toContact();
    if (!contact)
        return;

    if (action->context()->buddies().contains(m_myself->buddy()))
        return;

    auto account = contact.contactAccount();
    if (!account || !account.protocolHandler() || !account.protocolHandler()->rosterService())
        return;

    if (!account.protocolHandler()->isConnected())
        return;

    action->setEnabled(true);
}

#include "moc_subscription-action.cpp"
