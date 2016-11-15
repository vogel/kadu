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

#include "remove-subscription-action.h"

#include "protocols/services/subscription-service.h"

RemoveSubscriptionAction::RemoveSubscriptionAction(QObject *parent) :
		SubscriptionAction(parent)
{
	setType(ActionDescription::TypeUser);
	setName("rosterRemoveSubscription");
	setText(tr("Remove Subscription"));
}

RemoveSubscriptionAction::~RemoveSubscriptionAction()
{
}

void RemoveSubscriptionAction::init()
{
	registerAction(actionsRegistry());
}

void RemoveSubscriptionAction::execute(SubscriptionService *subscriptionService, const Contact &contact)
{
	subscriptionService->removeSubscription(contact);
}

#include "moc_remove-subscription-action.cpp"
