/*
 * %kadu copyright begin%
 * Copyright 2012 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include <stdio.h>
#include <qglobal.h>

extern "C" {
#	include <gcrypt.h>
#	include <libotr/userstate.h>
}

#include "accounts/account.h"
#include "accounts/account-manager.h"
#include "chat/chat.h"
#include "chat/type/chat-type-contact.h"
#include "contacts/contact-manager.h"
#include "contacts/contact-set.h"
#include "gui/widgets/chat-widget.h"
#include "gui/widgets/chat-widget-manager.h"
#include "message/message-manager.h"
#include "misc/kadu-paths.h"
#include "protocols/protocol.h"
#include "protocols/protocol-factory.h"
#include "protocols/services/chat-service.h"

#include "otr-fingerprint-service.h"
#include "otr-instance-tag-service.h"
#include "otr-op-data.h"
#include "otr-op-data-factory.h"
#include "otr-peer-identity-verification-service.h"
#include "otr-plugin.h"
#include "otr-policy.h"
#include "otr-policy-service.h"
#include "otr-private-key-service.h"
#include "otr-session-service.h"
#include "otr-instance-tag-service.h"
#include "otr-timer.h"
#include "otr-trust-level-service.h"
#include "otr-user-state-service.h"

#include "otr-app-ops-wrapper.h"

int kadu_otr_is_logged_in(void *opdata, const char *accountname, const char *protocol, const char *recipient)
{
	Q_UNUSED(accountname);
	Q_UNUSED(protocol);

	OtrOpData *opData = static_cast<OtrOpData *>(opdata);
	return (int)opData->appOpsWrapper()->isLoggedIn(opData, recipient);
}

void kadu_otr_inject_message(void *opdata, const char *accountname, const char *protocol, const char *recipient, const char *message)
{
	Q_UNUSED(accountname);
	Q_UNUSED(protocol);
	Q_UNUSED(recipient);

	OtrOpData *opData = static_cast<OtrOpData *>(opdata);
	opData->appOpsWrapper()->injectMessage(opData, QByteArray(message));
}

int kadu_otr_max_message_size(void *opdata, ConnContext *context)
{
	Q_UNUSED(context);

	OtrOpData *opData = static_cast<OtrOpData *>(opdata);
	return opData->appOpsWrapper()->maxMessageSize(opData);
}

const char * kadu_otr_otr_error_message(void *opdata, ConnContext *context, OtrlErrorCode err_code)
{
	Q_UNUSED(context);

	OtrOpData *opData = static_cast<OtrOpData *>(opdata);
	QString errorMessage = opData->appOpsWrapper()->errorMessage(opData, err_code);
	return strdup(errorMessage.toUtf8().constData());
}

void kadu_otr_otr_error_message_free(void *opdata, const char *err_msg)
{
	Q_UNUSED(opdata);

	free((char *)err_msg);
}

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

void kadu_otr_timer_control(void *opdata, unsigned int interval)
{
	Q_UNUSED(opdata);

	OtrPlugin *plugin = OtrPlugin::instance();
	if (!plugin)
		return;

	OtrTimer *otrTimer = plugin->otrTimer();
	if (!otrTimer)
		return;

	otrTimer->timerControl(interval);
}

OtrAppOpsWrapper::OtrAppOpsWrapper()
{
	Ops.policy = OtrPolicyService::wrapperOtrPolicy;
	Ops.create_privkey = OtrPrivateKeyService::wrapperOtrCreatePrivateKey;
	Ops.is_logged_in = kadu_otr_is_logged_in;
	Ops.inject_message = kadu_otr_inject_message;
	Ops.update_context_list = OtrTrustLevelService::wrapperOtrUpdateContextList;
	Ops.new_fingerprint = 0;
	Ops.write_fingerprints = OtrFingerprintService::wrapperOtrWriteFingerprints;
	Ops.gone_secure = OtrSessionService::wrapperOtrGoneSecure;
	Ops.gone_insecure = OtrSessionService::wrapperOtrGoneInsecure;
	Ops.still_secure = OtrSessionService::wrapperOtrStillSecure;
	Ops.max_message_size = kadu_otr_max_message_size;
	Ops.account_name = 0;
	Ops.account_name_free = 0;
	Ops.received_symkey = 0;
	Ops.otr_error_message = kadu_otr_otr_error_message;
	Ops.otr_error_message_free = kadu_otr_otr_error_message_free;
	Ops.resent_msg_prefix = kadu_otr_resent_msg_prefix;
	Ops.resent_msg_prefix_free = kadu_otr_resent_msg_prefix_free;
	Ops.handle_msg_event = kadu_otr_handle_msg_event;
	Ops.handle_smp_event = OtrPeerIdentityVerificationService::wrapperHandleSmpEvent;
	Ops.create_instag = OtrInstanceTagService::wrapperOtrCreateInstanceTag;
	Ops.convert_msg = 0;
	Ops.convert_free = 0;
	Ops.timer_control = kadu_otr_timer_control;
}

OtrAppOpsWrapper::~OtrAppOpsWrapper()
{
}

const OtrlMessageAppOps * OtrAppOpsWrapper::ops() const
{
	return &Ops;
}

OtrAppOpsWrapper::IsLoggedInStatus OtrAppOpsWrapper::isLoggedIn(OtrOpData *opData, const QString &contactId) const
{
	Account account = opData->contact().contactAccount();
	Contact contact = ContactManager::instance()->byId(account, contactId, ActionReturnNull);

	if (!contact)
		return NotSure;

	if (contact.currentStatus().isDisconnected())
		return NotLoggedIn;
	else
		return LoggedIn;
}

void OtrAppOpsWrapper::injectMessage(OtrOpData *opData, const QByteArray &messageContent) const
{
	Chat chat = ChatTypeContact::findChat(opData->contact(), ActionCreateAndAdd);
	MessageManager::instance()->sendRawMessage(chat, messageContent);
}

int OtrAppOpsWrapper::maxMessageSize(OtrOpData *opData) const
{
	Account account = opData->contact().contactAccount();
	Protocol *protocolHandler = account.protocolHandler();
	if (!protocolHandler)
		return 0;
	ChatService *chatService = protocolHandler->chatService();
	if (!chatService)
		return 0;
	return chatService->maxMessageLength();
}

QString OtrAppOpsWrapper::errorMessage(OtrOpData *opData, OtrlErrorCode errorCode) const
{
	Account account = opData->contact().contactAccount();
	Contact receiver = opData->contact();

	switch (errorCode)
	{
		case OTRL_ERRCODE_ENCRYPTION_ERROR:
			return tr("Error occurred during message encryption");
		case OTRL_ERRCODE_MSG_NOT_IN_PRIVATE:
			return tr("You sent encrypted data to %1, who wasn't expecting it").arg(receiver.display(true));
		case OTRL_ERRCODE_MSG_UNREADABLE:
			return tr("You transmitted an unreadable encrypted message");
		case OTRL_ERRCODE_MSG_MALFORMED:
			return tr("You transmitted a malformed data message");
		default:
			return QString();
	}
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
