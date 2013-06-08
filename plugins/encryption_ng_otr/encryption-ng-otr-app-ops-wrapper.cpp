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
#include "chat/chat.h"
#include "contacts/contact-manager.h"
#include "contacts/contact-set.h"
#include "gui/widgets/chat-widget.h"
#include "gui/widgets/chat-widget-manager.h"
#include "message/message-manager.h"
#include "misc/kadu-paths.h"
#include "protocols/protocol.h"
#include "protocols/protocol-factory.h"
#include "protocols/services/chat-service.h"

#include "encryption-ng-otr-notifier.h"
#include "encryption-ng-otr-op-data.h"
#include "encryption-ng-otr-plugin.h"
#include "encryption-ng-otr-policy.h"
#include "encryption-ng-otr-policy-account-store.h"
#include "encryption-ng-otr-private-key-service.h"
#include "encryption-ng-otr-timer.h"

#include "encryption-ng-otr-app-ops-wrapper.h"

OtrlPolicy kadu_enomf_policy(void *opdata, ConnContext *context)
{
	Q_UNUSED(context);

	EncryptionNgOtrOpData *ngOtrOpData = static_cast<EncryptionNgOtrOpData *>(opdata);
	return ngOtrOpData->appOpsWrapper()->policy(ngOtrOpData);
}

void kadu_enomf_create_privkey(void *opdata, const char *accountname, const char *protocol)
{
	Q_UNUSED(accountname);
	Q_UNUSED(protocol);

	EncryptionNgOtrOpData *ngOtrOpData = static_cast<EncryptionNgOtrOpData *>(opdata);
	ngOtrOpData->appOpsWrapper()->createPrivateKey(ngOtrOpData);
}

int kadu_enomf_is_logged_in(void *opdata, const char *accountname, const char *protocol, const char *recipient)
{
	Q_UNUSED(accountname);
	Q_UNUSED(protocol);

	EncryptionNgOtrOpData *ngOtrOpData = static_cast<EncryptionNgOtrOpData *>(opdata);
	return (int)ngOtrOpData->appOpsWrapper()->isLoggedIn(ngOtrOpData, recipient);
}

void kadu_enomf_inject_message(void *opdata, const char *accountname, const char *protocol, const char *recipient, const char *message)
{
	Q_UNUSED(accountname);
	Q_UNUSED(protocol);
	Q_UNUSED(recipient);

	EncryptionNgOtrOpData *ngOtrOpData = static_cast<EncryptionNgOtrOpData *>(opdata);
	ngOtrOpData->appOpsWrapper()->injectMessage(ngOtrOpData, QString::fromUtf8(message));
}

void kadu_enomf_update_context_list(void *opdata)
{
    Q_UNUSED(opdata);
}

void kadu_enomf_new_fingerprint(void *opdata, OtrlUserState us, const char *accountname, const char *protocol,
								const char *username, unsigned char fingerprint[20])
{
	Q_UNUSED(opdata);
	Q_UNUSED(us);
	Q_UNUSED(accountname);
	Q_UNUSED(protocol);
	Q_UNUSED(username);
	Q_UNUSED(fingerprint);
}

void kadu_enomf_write_fingerprints(void *opdata)
{
	Q_UNUSED(opdata);
}

void kadu_enomf_gone_secure(void *opdata, ConnContext *context)
{
	Q_UNUSED(context);

	EncryptionNgOtrOpData *ngOtrOpData = static_cast<EncryptionNgOtrOpData *>(opdata);
	ngOtrOpData->appOpsWrapper()->goneSecure(ngOtrOpData);
}

void kadu_enomf_gone_insecure(void *opdata, ConnContext *context)
{
	Q_UNUSED(context);

	EncryptionNgOtrOpData *ngOtrOpData = static_cast<EncryptionNgOtrOpData *>(opdata);
	ngOtrOpData->appOpsWrapper()->goneInsecure(ngOtrOpData);
}

void kadu_enomf_still_secure(void *opdata, ConnContext *context, int is_reply)
{
	Q_UNUSED(context);
	Q_UNUSED(is_reply);

	EncryptionNgOtrOpData *ngOtrOpData = static_cast<EncryptionNgOtrOpData *>(opdata);
	ngOtrOpData->appOpsWrapper()->stillSecure(ngOtrOpData);
}

int kadu_enomf_max_message_size(void *opdata, ConnContext *context)
{
	Q_UNUSED(context);

	EncryptionNgOtrOpData *ngOtrOpData = static_cast<EncryptionNgOtrOpData *>(opdata);
	return ngOtrOpData->appOpsWrapper()->maxMessageSize(ngOtrOpData);
}

const char * kadu_enomf_otr_error_message(void *opdata, ConnContext *context, OtrlErrorCode err_code)
{
	Q_UNUSED(context);

	EncryptionNgOtrOpData *ngOtrOpData = static_cast<EncryptionNgOtrOpData *>(opdata);
	QString errorMessage = ngOtrOpData->appOpsWrapper()->errorMessage(ngOtrOpData, err_code);
	return strdup(errorMessage.toUtf8().constData());
}

void kadu_enomf_otr_error_message_free(void *opdata, const char *err_msg)
{
	Q_UNUSED(opdata);

	free((char *)err_msg);
}

const char * kadu_enomf_resent_msg_prefix(void *opdata, ConnContext *context)
{
	Q_UNUSED(opdata);
	Q_UNUSED(context);

	EncryptionNgOtrOpData *ngOtrOpData = static_cast<EncryptionNgOtrOpData *>(opdata);
	QString resentMessagePrefix = ngOtrOpData->appOpsWrapper()->resentMessagePrefix();
	return strdup(resentMessagePrefix.toUtf8().constData());
}

void kadu_enomf_resent_msg_prefix_free(void *opdata, const char *prefix)
{
	Q_UNUSED(opdata);

	free((char *)prefix);
}

void kadu_enomf_handle_smp_event(void *opdata, OtrlSMPEvent smp_event, ConnContext *context,
								 unsigned short progress_percent, char *question)
{
	Q_UNUSED(opdata);
	Q_UNUSED(smp_event);
	Q_UNUSED(context);
	Q_UNUSED(progress_percent);
	Q_UNUSED(question);
}

void kadu_enomf_create_instag(void *opdata, const char *accountname, const char *protocol)
{
	Q_UNUSED(accountname);
	Q_UNUSED(protocol);

	EncryptionNgOtrOpData *ngOtrOpData = static_cast<EncryptionNgOtrOpData *>(opdata);
	ngOtrOpData->appOpsWrapper()->createInstanceTag(ngOtrOpData);
}

void kadu_enomf_handle_msg_event(void *opdata, OtrlMessageEvent msg_event, ConnContext *context,
								 const char *message, gcry_error_t err)
{
	Q_UNUSED(opdata);
	Q_UNUSED(msg_event);
	Q_UNUSED(context);
	Q_UNUSED(message);
	Q_UNUSED(err);

	EncryptionNgOtrOpData *ngOtrOpData = static_cast<EncryptionNgOtrOpData *>(opdata);
	ngOtrOpData->appOpsWrapper()->handleMsgEvent(ngOtrOpData, msg_event, message, err);
	
	
}

void kadu_enomf_timer_control(void *opdata, unsigned int interval)
{
	Q_UNUSED(opdata);

	EncryptionNgOtrPlugin *plugin = EncryptionNgOtrPlugin::instance();
	if (!plugin)
		return;

	EncryptionNgOtrTimer *otrTimer = plugin->otrTimer();
	if (!otrTimer)
		return;

	otrTimer->timerControl(interval);
}

EncryptionNgOtrAppOpsWrapper::EncryptionNgOtrAppOpsWrapper() :
		UserState(0)
{
	Ops.policy = kadu_enomf_policy;
	Ops.create_privkey = kadu_enomf_create_privkey;
	Ops.is_logged_in = kadu_enomf_is_logged_in;
	Ops.inject_message = kadu_enomf_inject_message;
	Ops.update_context_list = kadu_enomf_update_context_list;
	Ops.new_fingerprint = kadu_enomf_new_fingerprint;
	Ops.write_fingerprints = kadu_enomf_write_fingerprints;
	Ops.gone_secure = kadu_enomf_gone_secure;
	Ops.gone_insecure = kadu_enomf_gone_insecure;
	Ops.still_secure = kadu_enomf_still_secure;
	Ops.max_message_size = kadu_enomf_max_message_size;
	Ops.account_name = 0;
	Ops.account_name_free = 0;
	Ops.received_symkey = 0;
	Ops.otr_error_message = kadu_enomf_otr_error_message;
	Ops.otr_error_message_free = kadu_enomf_otr_error_message_free;
	Ops.resent_msg_prefix = kadu_enomf_resent_msg_prefix;
	Ops.resent_msg_prefix_free = kadu_enomf_resent_msg_prefix_free;
	Ops.handle_msg_event = kadu_enomf_handle_msg_event;
	Ops.handle_smp_event = kadu_enomf_handle_smp_event;
	Ops.create_instag = kadu_enomf_create_instag;
	Ops.convert_msg = 0;
	Ops.convert_free = 0;
	Ops.timer_control = kadu_enomf_timer_control;
}

EncryptionNgOtrAppOpsWrapper::~EncryptionNgOtrAppOpsWrapper()
{
}

void EncryptionNgOtrAppOpsWrapper::setUserState(EncryptionNgOtrUserState *userState)
{
	UserState = userState;
}

const OtrlMessageAppOps * EncryptionNgOtrAppOpsWrapper::ops() const
{
	return &Ops;
}

OtrlPolicy EncryptionNgOtrAppOpsWrapper::policy(EncryptionNgOtrOpData *ngOtrOpData) const
{
	Account account = ngOtrOpData->message().messageChat().chatAccount();
	EncryptionNgOtrPolicy policy = EncryptionNgOtrPolicyAccountStore::loadPolicyFromAccount(account);

	return policy.toOtrPolicy();
}

void EncryptionNgOtrAppOpsWrapper::createPrivateKey(EncryptionNgOtrOpData *ngOtrOpData) const
{
	Account account = ngOtrOpData->message().messageChat().chatAccount();
	ngOtrOpData->privateKeyService()->createPrivateKey(account);
}

EncryptionNgOtrAppOpsWrapper::IsLoggedInStatus EncryptionNgOtrAppOpsWrapper::isLoggedIn(EncryptionNgOtrOpData *ngOtrOpData, const QString &contactId) const
{
	Account account = ngOtrOpData->message().messageChat().chatAccount();
	Contact contact = ContactManager::instance()->byId(account, contactId, ActionReturnNull);

	if (!contact)
		return NotSure;

	if (contact.currentStatus().isDisconnected())
		return NotLoggedIn;
	else
		return LoggedIn;
}

void EncryptionNgOtrAppOpsWrapper::injectMessage(EncryptionNgOtrOpData *ngOtrOpData, const QString &messageContent) const
{
	Chat chat = ngOtrOpData->message().messageChat();
	MessageManager::instance()->sendMessage(chat, messageContent, true);
}

void EncryptionNgOtrAppOpsWrapper::goneSecure(EncryptionNgOtrOpData *ngOtrOpData) const
{
	Chat chat = ngOtrOpData->message().messageChat();
	ngOtrOpData->notifier()->notifyGoneSecure(chat);
}

void EncryptionNgOtrAppOpsWrapper::goneInsecure(EncryptionNgOtrOpData *ngOtrOpData) const
{
	Chat chat = ngOtrOpData->message().messageChat();
	ngOtrOpData->notifier()->notifyGoneInsecure(chat);
}

void EncryptionNgOtrAppOpsWrapper::stillSecure(EncryptionNgOtrOpData *ngOtrOpData) const
{
	Chat chat = ngOtrOpData->message().messageChat();
	ngOtrOpData->notifier()->notifyStillSecure(chat);
}

int EncryptionNgOtrAppOpsWrapper::maxMessageSize(EncryptionNgOtrOpData *ngOtrOpData) const
{
	Account account = ngOtrOpData->message().messageChat().chatAccount();
	Protocol *protocolHandler = account.protocolHandler();
	if (!protocolHandler)
		return 0;
	ChatService *chatService = protocolHandler->chatService();
	if (!chatService)
		return 0;
	return chatService->maxMessageLength();
}

QString EncryptionNgOtrAppOpsWrapper::errorMessage(EncryptionNgOtrOpData *ngOtrOpData, OtrlErrorCode errorCode) const
{
	Account account = ngOtrOpData->message().messageChat().chatAccount();
	Contact receiver = ngOtrOpData->message().messageChat().contacts().toContact();

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

QString EncryptionNgOtrAppOpsWrapper::resentMessagePrefix() const
{
	return tr("[resent]");
}

void EncryptionNgOtrAppOpsWrapper::handleMsgEvent(EncryptionNgOtrOpData *ngOtrOpData, OtrlMessageEvent event,
												  const QString &message, gcry_error_t errorCode) const
{
	QString senderDisplay = ngOtrOpData->message().messageSender().display(true);
	QString errorMessage = messageString(event, message, errorCode, senderDisplay);

	if (errorMessage.isEmpty())
		return;

	Chat chat = ngOtrOpData->message().messageChat();
	ChatWidget *chatWidget = ChatWidgetManager::instance()->byChat(chat, false);
	if (chatWidget)
		chatWidget->appendSystemMessage(errorMessage);
}

QString EncryptionNgOtrAppOpsWrapper::messageString(OtrlMessageEvent event, const QString &message, gcry_error_t errorCode, const QString &senderDisplay) const
{
	switch (event)
	{
		case OTRL_MSGEVENT_ENCRYPTION_REQUIRED:
			return tr("Unencrypted messages to %1 are not allowed. Attmpting to start a private conversation...").arg(senderDisplay);
		case OTRL_MSGEVENT_ENCRYPTION_ERROR:
			return tr("Encryption error. Message was not sent.");
		case OTRL_MSGEVENT_CONNECTION_ENDED:
			return tr("Message was not sent. Private conversation was closed by %1. Either restart or close your private conversation.").arg(senderDisplay);
		case OTRL_MSGEVENT_SETUP_ERROR:
			return tr("Error during setting up private conversation with %1: %2").arg(senderDisplay).arg(gpgErrorString(errorCode));
		case OTRL_MSGEVENT_MSG_REFLECTED:
			return tr("We are receiving our own OTR messages from %1. Either restart or close your private conversation.").arg(senderDisplay);
		case OTRL_MSGEVENT_MSG_RESENT:
			return tr("Last message was resent: %1").arg(message);
		case OTRL_MSGEVENT_RCVDMSG_NOT_IN_PRIVATE:
			return tr("Message from %1 was unreadable, as you are not currently communicating privately.").arg(senderDisplay);
		case OTRL_MSGEVENT_RCVDMSG_UNREADABLE:
			return tr("Message from %1 was unreadable.").arg(senderDisplay);
		case OTRL_MSGEVENT_RCVDMSG_MALFORMED:
			return tr("Message from %1 was malformed.").arg(senderDisplay);
		case OTRL_MSGEVENT_RCVDMSG_GENERAL_ERR:
			return tr("Encryption error: %1").arg(message);
		case OTRL_MSGEVENT_RCVDMSG_UNENCRYPTED:
			return tr("Message from %1 was unencrypted.").arg(senderDisplay);
		case OTRL_MSGEVENT_RCVDMSG_UNRECOGNIZED:
			return tr("Message from %1 was unrecognized.").arg(senderDisplay);
		case OTRL_MSGEVENT_RCVDMSG_FOR_OTHER_INSTANCE:
			return tr("%1 has sent a message intended for different session. "
					  "If you are logged in multiple times another session may have received the message.").arg(senderDisplay);
		default:
			return QString();
	}
}

QString EncryptionNgOtrAppOpsWrapper::gpgErrorString(gcry_error_t errorCode) const
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

void EncryptionNgOtrAppOpsWrapper::createInstanceTag(EncryptionNgOtrOpData *ngOtrOpData)
{
	Q_ASSERT(UserState);

	Account account = ngOtrOpData->message().messageChat().chatAccount();
	QString fileName = instanceTagsFileName();

	otrl_instag_generate(UserState->userState(), fileName.toUtf8().data(), account.id().toUtf8().data(), account.protocolName().toUtf8().data());
}

QString EncryptionNgOtrAppOpsWrapper::instanceTagsFileName() const
{
	return KaduPaths::instance()->profilePath() + QString("/keys/otr_instance_tags");
}
