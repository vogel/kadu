/*
 * %kadu copyright begin%
 * Copyright 2013 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "otr-error-message-service.h"
#include "otr-fingerprint-service.h"
#include "otr-instance-tag-service.h"
#include "otr-is-logged-in-service.h"
#include "otr-message-event-service.h"
#include "otr-message-service.h"
#include "otr-peer-identity-verification-service.h"
#include "otr-policy-service.h"
#include "otr-private-key-service.h"
#include "otr-session-service.h"
#include "otr-timer-service.h"
#include "otr-trust-level-service.h"

#include "otr-app-ops-service.h"

OtrAppOpsService::OtrAppOpsService()
{
	AppOps.otr_error_message = OtrErrorMessageService::wrapperOtrErrorMessage;
	AppOps.otr_error_message_free = OtrErrorMessageService::wrapperOtrErrorMessageFree;
	AppOps.write_fingerprints = OtrFingerprintService::wrapperOtrWriteFingerprints;
	AppOps.is_logged_in = OtrIsLoggedInService::wrapperOtrIsLoggedIn;
	AppOps.handle_msg_event = OtrMessageEventService::wrapperOtrHandleMessageEvent;
	AppOps.inject_message = OtrMessageService::wrapperOtrInjectMessage;
	AppOps.max_message_size = OtrMessageService::wrapperOtrMaxMessageSize;
	AppOps.resent_msg_prefix = OtrMessageService::wrapperOtrResentMessagePrefix;
	AppOps.resent_msg_prefix_free = OtrMessageService::wrapperOtrResentMessagePrefixFree;
	AppOps.handle_smp_event = OtrPeerIdentityVerificationService::wrapperHandleSmpEvent;
	AppOps.policy = OtrPolicyService::wrapperOtrPolicy;
	AppOps.create_privkey = OtrPrivateKeyService::wrapperOtrCreatePrivateKey;
	AppOps.gone_secure = OtrSessionService::wrapperOtrGoneSecure;
	AppOps.gone_insecure = OtrSessionService::wrapperOtrGoneInsecure;
	AppOps.still_secure = OtrSessionService::wrapperOtrStillSecure;
	AppOps.create_instag = OtrInstanceTagService::wrapperOtrCreateInstanceTag;
	AppOps.timer_control = OtrTimerService::wrapperOtrTimerControl;
	AppOps.update_context_list = OtrTrustLevelService::wrapperOtrUpdateContextList;

	AppOps.new_fingerprint = 0;
	AppOps.account_name = 0;
	AppOps.account_name_free = 0;
	AppOps.received_symkey = 0;
	AppOps.convert_msg = 0;
	AppOps.convert_free = 0;
}

OtrAppOpsService::~OtrAppOpsService()
{
}

const OtrlMessageAppOps * OtrAppOpsService::appOps() const
{
	return &AppOps;
}
