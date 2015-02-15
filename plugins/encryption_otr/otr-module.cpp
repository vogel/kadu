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

#include "otr-module.h"

#include "gui/widgets/otr-account-configuration-widget-factory.h"
#include "gui/widgets/otr-buddy-configuration-widget-factory.h"
#include "gui/widgets/otr-chat-top-bar-widget-factory.h"
#include "gui/windows/otr-peer-identity-verification-window-factory.h"
#include "gui/windows/otr-peer-identity-verification-window-repository.h"
#include "otr-app-ops-service.h"
#include "otr-context-converter.h"
#include "otr-error-message-service.h"
#include "otr-fingerprint-service.h"
#include "otr-instance-tag-service.h"
#include "otr-is-logged-in-service.h"
#include "otr-message-event-service.h"
#include "otr-message-service.h"
#include "otr-notifier.h"
#include "otr-op-data-factory.h"
#include "otr-path-service.h"
#include "otr-peer-identity-verification-service.h"
#include "otr-policy-service.h"
#include "otr-private-key-service.h"
#include "otr-raw-message-transformer.h"
#include "otr-session-service.h"
#include "otr-timer-service.h"
#include "otr-trust-level-service.h"
#include "otr-user-state-service.h"

OtrModule::OtrModule()
{
	add_type<OtrAccountConfigurationWidgetFactory>();
	add_type<OtrAppOpsService>();
	add_type<OtrBuddyConfigurationWidgetFactory>();
	add_type<OtrChatTopBarWidgetFactory>();
	add_type<OtrContextConverter>();
	add_type<OtrErrorMessageService>();
	add_type<OtrFingerprintService>();
	add_type<OtrInstanceTagService>();
	add_type<OtrIsLoggedInService>();
	add_type<OtrMessageEventService>();
	add_type<OtrMessageService>();
	add_type<OtrNotifier>();
	add_type<OtrOpDataFactory>();
	add_type<OtrPathService>();
	add_type<OtrPeerIdentityVerificationService>();
	add_type<OtrPeerIdentityVerificationWindowFactory>();
	add_type<OtrPeerIdentityVerificationWindowRepository>();
	add_type<OtrPolicyService>();
	add_type<OtrPrivateKeyService>();
	add_type<OtrRawMessageTransformer>();
	add_type<OtrSessionService>();
	add_type<OtrTimerService>();
	add_type<OtrTrustLevelService>();
	add_type<OtrUserStateService>();
}

OtrModule::~OtrModule()
{
}
