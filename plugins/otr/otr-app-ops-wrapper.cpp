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

extern "C" {
#	include <gcrypt.h>
}

#include "chat/type/chat-type-contact.h"
#include "contacts/contact-manager.h"
#include "gui/widgets/chat-widget.h"
#include "gui/widgets/chat-widget-manager.h"

#include "otr-error-message-service.h"
#include "otr-fingerprint-service.h"
#include "otr-is-logged-in-service.h"
#include "otr-message-event-service.h"
#include "otr-message-service.h"
#include "otr-op-data.h"
#include "otr-peer-identity-verification-service.h"
#include "otr-plugin.h"
#include "otr-policy-service.h"
#include "otr-private-key-service.h"
#include "otr-session-service.h"
#include "otr-instance-tag-service.h"
#include "otr-timer-service.h"
#include "otr-trust-level-service.h"

#include "otr-app-ops-wrapper.h"

OtrAppOpsWrapper::OtrAppOpsWrapper()
{
	Ops.policy = OtrPolicyService::wrapperOtrPolicy;
	Ops.create_privkey = OtrPrivateKeyService::wrapperOtrCreatePrivateKey;
	Ops.is_logged_in = OtrIsLoggedInService::wrapperOtrIsLoggedIn;
	Ops.inject_message = OtrMessageService::wrapperOtrInjectMessage;
	Ops.update_context_list = OtrTrustLevelService::wrapperOtrUpdateContextList;
	Ops.new_fingerprint = 0;
	Ops.write_fingerprints = OtrFingerprintService::wrapperOtrWriteFingerprints;
	Ops.gone_secure = OtrSessionService::wrapperOtrGoneSecure;
	Ops.gone_insecure = OtrSessionService::wrapperOtrGoneInsecure;
	Ops.still_secure = OtrSessionService::wrapperOtrStillSecure;
	Ops.max_message_size = OtrMessageService::wrapperOtrMaxMessageSize;
	Ops.account_name = 0;
	Ops.account_name_free = 0;
	Ops.received_symkey = 0;
	Ops.otr_error_message = OtrErrorMessageService::wrapperOtrErrorMessage;
	Ops.otr_error_message_free = OtrErrorMessageService::wrapperOtrErrorMessageFree;
	Ops.resent_msg_prefix = OtrMessageService::wrapperOtrResentMessagePrefix;
	Ops.resent_msg_prefix_free = OtrMessageService::wrapperOtrResentMessagePrefixFree;
	Ops.handle_msg_event = OtrMessageEventService::wrapperOtrHandleMessageEvent;
	Ops.handle_smp_event = OtrPeerIdentityVerificationService::wrapperHandleSmpEvent;
	Ops.create_instag = OtrInstanceTagService::wrapperOtrCreateInstanceTag;
	Ops.convert_msg = 0;
	Ops.convert_free = 0;
	Ops.timer_control = OtrTimerService::wrapperOtrTimerControl;
}

OtrAppOpsWrapper::~OtrAppOpsWrapper()
{
}

const OtrlMessageAppOps * OtrAppOpsWrapper::ops() const
{
	return &Ops;
}
