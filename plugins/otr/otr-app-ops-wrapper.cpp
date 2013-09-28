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

const char * kadu_otr_resent_msg_prefix(void *opdata, ConnContext *context)
{
	Q_UNUSED(opdata);
	Q_UNUSED(context);

	OtrOpData *opData = static_cast<OtrOpData *>(opdata);
	QString resentMessagePrefix = opData->appOpsWrapper()->resentMessagePrefix();
	return strdup(resentMessagePrefix.toUtf8().constData());
}

void kadu_otr_resent_msg_prefix_free(void *opdata, const char *prefix)
{
	Q_UNUSED(opdata);

	free((char *)prefix);
}

void kadu_otr_handle_msg_event(void *opdata, OtrlMessageEvent msg_event, ConnContext *context,
								 const char *message, gcry_error_t err)
{
	Q_UNUSED(opdata);
	Q_UNUSED(msg_event);
	Q_UNUSED(context);
	Q_UNUSED(message);
	Q_UNUSED(err);

	OtrOpData *opData = static_cast<OtrOpData *>(opdata);
	opData->appOpsWrapper()->handleMsgEvent(opData, msg_event, message, err);
}

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
	Ops.resent_msg_prefix = kadu_otr_resent_msg_prefix;
	Ops.resent_msg_prefix_free = kadu_otr_resent_msg_prefix_free;
	Ops.handle_msg_event = kadu_otr_handle_msg_event;
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

QString OtrAppOpsWrapper::resentMessagePrefix() const
{
	return tr("[resent]");
}

void OtrAppOpsWrapper::handleMsgEvent(OtrOpData *opData, OtrlMessageEvent event,
									  const QString &message, gcry_error_t errorCode) const
{
	QString peerDisplay = opData->peerDisplay();
	QString errorMessage = messageString(event, message, errorCode, peerDisplay);

	if (errorMessage.isEmpty())
		return;

	Chat chat = ChatTypeContact::findChat(opData->contact(), ActionCreateAndAdd);
	ChatWidget *chatWidget = ChatWidgetManager::instance()->byChat(chat, false);
	if (chatWidget)
		chatWidget->appendSystemMessage(errorMessage);
}

QString OtrAppOpsWrapper::messageString(OtrlMessageEvent event, const QString &message, gcry_error_t errorCode, const QString &peerDisplay) const
{
	switch (event)
	{
		case OTRL_MSGEVENT_ENCRYPTION_REQUIRED:
			return tr("Unencrypted messages to %1 are not allowed. Attmpting to start a private conversation...").arg(peerDisplay);
		case OTRL_MSGEVENT_ENCRYPTION_ERROR:
			return tr("Encryption error. Message was not sent.");
		case OTRL_MSGEVENT_CONNECTION_ENDED:
			return tr("Message was not sent. Private conversation was closed by %1. Either restart or close your private conversation.").arg(peerDisplay);
		case OTRL_MSGEVENT_SETUP_ERROR:
			return tr("Error during setting up private conversation with %1: %2").arg(peerDisplay).arg(gpgErrorString(errorCode));
		case OTRL_MSGEVENT_MSG_REFLECTED:
			return tr("We are receiving our own OTR messages from %1. Either restart or close your private conversation.").arg(peerDisplay);
		case OTRL_MSGEVENT_MSG_RESENT:
			return tr("Last message was resent: %1").arg(message);
		case OTRL_MSGEVENT_RCVDMSG_NOT_IN_PRIVATE:
			return tr("Message from %1 was unreadable, as you are not currently communicating privately.").arg(peerDisplay);
		case OTRL_MSGEVENT_RCVDMSG_UNREADABLE:
			return tr("Message from %1 was unreadable.").arg(peerDisplay);
		case OTRL_MSGEVENT_RCVDMSG_MALFORMED:
			return tr("Message from %1 was malformed.").arg(peerDisplay);
		case OTRL_MSGEVENT_RCVDMSG_GENERAL_ERR:
			return tr("Encryption error: %1").arg(message);
		case OTRL_MSGEVENT_RCVDMSG_UNENCRYPTED:
			return tr("Message from %1 was unencrypted.").arg(peerDisplay);
		case OTRL_MSGEVENT_RCVDMSG_UNRECOGNIZED:
			return tr("Message from %1 was unrecognized.").arg(peerDisplay);
		case OTRL_MSGEVENT_RCVDMSG_FOR_OTHER_INSTANCE:
			return tr("%1 has sent a message intended for different session. "
					  "If you are logged in multiple times another session may have received the message.").arg(peerDisplay);
		default:
			return QString();
	}
}

QString OtrAppOpsWrapper::gpgErrorString(gcry_error_t errorCode) const
{
	switch (errorCode)
	{
		case 0:
		case GPG_ERR_INV_VALUE:
			return (tr("Malformed message received"));
		default:
			return gcry_strerror(errorCode);
	}
}
