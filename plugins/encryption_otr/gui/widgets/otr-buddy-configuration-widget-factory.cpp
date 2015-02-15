/*
 * %kadu copyright begin%
 * Copyright 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "gui/widgets/otr-buddy-configuration-widget.h"
#include "otr-policy-service.h"

#include "otr-buddy-configuration-widget-factory.h"

OtrBuddyConfigurationWidgetFactory::~OtrBuddyConfigurationWidgetFactory()
{
}

void OtrBuddyConfigurationWidgetFactory::setPolicyService(OtrPolicyService *policyService)
{
	PolicyService = policyService;
}

BuddyConfigurationWidget * OtrBuddyConfigurationWidgetFactory::createWidget(const Buddy &buddy, QWidget *parent)
{
	OtrBuddyConfigurationWidget *result = new OtrBuddyConfigurationWidget(buddy, parent);
	result->setPolicyService(PolicyService.data());

	return result;
}
