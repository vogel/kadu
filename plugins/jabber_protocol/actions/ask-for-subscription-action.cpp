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

#include "ask-for-subscription-action.h"
#include "ask-for-subscription-action.moc"

#include "protocols/services/subscription-service.h"

AskForSubscriptionAction::AskForSubscriptionAction(QObject *parent) : SubscriptionAction(parent)
{
    setType(ActionDescription::TypeUser);
    setName("rosterAskForSubscription");
    setText(tr("Ask for Subscription"));
}

AskForSubscriptionAction::~AskForSubscriptionAction()
{
}

void AskForSubscriptionAction::execute(SubscriptionService *subscriptionService, const Contact &contact)
{
    subscriptionService->requestSubscription(contact);
}
