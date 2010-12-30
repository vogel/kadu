/*
 * %kadu copyright begin%
 * Copyright 2010 Piotr Galiszewski (piotr.galiszewski@kadu.im)
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

#include "mobile-number-manager.h"
#include "sms-actions.h"
#include "sms-configuration-ui-handler.h"
#include "sms-gateway-manager.h"

extern "C" KADU_EXPORT int sms_init(bool firstLoad)
{
	SmsConfigurationUiHandler::registerConfigurationUi();
	SmsActions::registerActions(firstLoad);

	return 0;
}

extern "C" KADU_EXPORT void sms_close()
{
	SmsGatewayManager::destroyInstance();
	MobileNumberManager::destroyInstance();
	SmsActions::unregisterActions();
	SmsConfigurationUiHandler::unregisterConfigurationUi();
}
