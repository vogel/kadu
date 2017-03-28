/*
 * %kadu copyright begin%
 * Copyright 2015 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "sms-module.h"

#include "gui/windows/sms-dialog-repository.h"
#include "mobile-number-manager.h"
#include "scripts/sms-script-manager.h"
#include "send-sms-action.h"
#include "sms-actions.h"
#include "sms-configuration-ui-handler.h"
#include "sms-gateway-manager.h"
#include "sms-plugin-object.h"

SmsModule::SmsModule()
{
    add_type<MobileNumberManager>();
    add_type<SendSmsAction>();
    add_type<SmsActions>();
    add_type<SmsConfigurationUiHandler>();
    add_type<SmsDialogRepository>();
    add_type<SmsGatewayManager>();
    add_type<SmsPluginObject>();
    add_type<SmsScriptsManager>();
}

SmsModule::~SmsModule()
{
}
