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

#include "otr-context-converter.h"
#include "otr-fingerprint-service.h"
#include "otr-op-data.h"
#include "otr-plugin.h"
#include "otr-policy.h"
#include "otr-policy-account-store.h"
#include "otr-private-key-service.h"
#include "otr-timer.h"
#include "otr-trust-level.h"
#include "otr-trust-level-contact-store.h"
#include "otr-trust-level-service.h"

#include "otr-app-ops-wrapper.h"

OtrlPolicy kadu_otr_policy(void *opdata, ConnContext *context)
{
	Q_UNUSED(context);

	OtrOpData *otrOpData = static_cast<OtrOpData *>(opdata);
	return otrOpData->appOpsWrapper()->policy(otrOpData);
}

void kadu_otr_create_privkey(void *opdata, const char *accountname, const char *protocol)
{
	Q_UNUSED(accountname);
	Q_UNUSED(protocol);

	OtrOpData *otrOpData = static_cast<OtrOpData *>(opdata);
	otrOpData->appOpsWrapper()->createPrivateKey(otrOpData);
}

int kadu_otr_is_logged_in(void *opdata, const char *accountname, const char *protocol, const char *recipient)
{
	Q_UNUSED(accountname);
	Q_UNUSED(protocol);

	OtrOpData *otrOpData = static_cast<OtrOpData *>(opdata);
	return (int)otrOpData->appOpsWrapper()->isLoggedIn(otrOpData, recipient);
}

void kadu_otr_inject_message(void *opdata, const char *accountname, const char *protocol, const char *recipient, const char *message)
{
	Q_UNUSED(accountname);
	Q_UNUSED(protocol);
	Q_UNUSED(recipient);

	OtrOpData *otrOpData = static_cast<OtrOpData *>(opdata);
	otrOpData->appOpsWrapper()->injectMessage(otrOpData, QByteArray(message));
}

void kadu_otr_update_context_list(void *opdata)
{
	OtrOpData *otrOpData = static_cast<OtrOpData *>(opdata);
	otrOpData->appOpsWrapper()->updateContextList(otrOpData);
}

void kadu_otr_new_fingerprint(void *opdata, OtrlUserState us, const char *accountname, const char *protocol,
								const char *username, unsigned char fingerprint[20])
{
	Q_UNUSED(opdata);
	Q_UNUSED(us);
	Q_UNUSED(accountname);
	Q_UNUSED(protocol);
	Q_UNUSED(username);
	Q_UNUSED(fingerprint);
}

void kadu_otr_write_fingerprints(void *opdata)
{
	OtrOpData *otrOpData = static_cast<OtrOpData *>(opdata);
	otrOpData->appOpsWrapper()->writeFingerprints();
}

void kadu_otr_gone_secure(void *opdata, ConnContext *context)
{
	Q_UNUSED(context);

	OtrOpData *otrOpData = static_cast<OtrOpData *>(opdata);
	otrOpData->appOpsWrapper()->goneSecure(otrOpData);
}

void kadu_otr_gone_insecure(void *opdata, ConnContext *context)
{
	Q_UNUSED(context);

	OtrOpData *otrOpData = static_cast<OtrOpData *>(opdata);
	otrOpData->appOpsWrapper()->goneInsecure(otrOpData);
}

void kadu_otr_still_secure(void *opdata, ConnContext *context, int is_reply)
{
	Q_UNUSED(context);
	Q_UNUSED(is_reply);

	OtrOpData *otrOpData = static_cast<OtrOpData *>(opdata);
	otrOpData->appOpsWrapper()->stillSecure(otrOpData);
}

int kadu_otr_max_message_size(void *opdata, ConnContext *context)
{
	Q_UNUSED(context);

	OtrOpData *otrOpData = static_cast<OtrOpData *>(opdata);
	return otrOpData->appOpsWrapper()->maxMessageSize(otrOpData);
}

const char * kadu_otr_otr_error_message(void *opdata, ConnContext *context, OtrlErrorCode err_code)
{
	Q_UNUSED(context);

	OtrOpData *otrOpData = static_cast<OtrOpData *>(opdata);
	QString errorMessage = otrOpData->appOpsWrapper()->errorMessage(otrOpData, err_code);
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

	OtrOpData *otrOpData = static_cast<OtrOpData *>(opdata);
	QString resentMessagePrefix = otrOpData->appOpsWrapper()->resentMessagePrefix();
	return strdup(resentMessagePrefix.toUtf8().constData());
}

void kadu_otr_resent_msg_prefix_free(void *opdata, const char *prefix)
{
	Q_UNUSED(opdata);

	free((char *)prefix);
}

void kadu_otr_handle_smp_event(void *opdata, OtrlSMPEvent smp_event, ConnContext *context,
								 unsigned short progress_percent, char *question)
{
	Q_UNUSED(opdata);
	Q_UNUSED(smp_event);
	Q_UNUSED(context);
	Q_UNUSED(progress_percent);
	Q_UNUSED(question);
}

void kadu_otr_create_instag(void *opdata, const char *accountname, const char *protocol)
{
	Q_UNUSED(accountname);
	Q_UNUSED(protocol);

	OtrOpData *otrOpData = static_cast<OtrOpData *>(opdata);
	otrOpData->appOpsWrapper()->createInstanceTag(otrOpData);
}

void kadu_otr_handle_msg_event(void *opdata, OtrlMessageEvent msg_event, ConnContext *context,
								 const char *message, gcry_error_t err)
{
	Q_UNUSED(opdata);
	Q_UNUSED(msg_event);
	Q_UNUSED(context);
	Q_UNUSED(message);
	Q_UNUSED(err);

	OtrOpData *otrOpData = static_cast<OtrOpData *>(opdata);
	otrOpData->appOpsWrapper()->handleMsgEvent(otrOpData, msg_event, message, err);
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

OtrAppOpsWrapper::OtrAppOpsWrapper() :
		UserState(0)
{
	Ops.policy = kadu_otr_policy;
	Ops.create_privkey = kadu_otr_create_privkey;
	Ops.is_logged_in = kadu_otr_is_logged_in;
	Ops.inject_message = kadu_otr_inject_message;
	Ops.update_context_list = kadu_otr_update_context_list;
	Ops.new_fingerprint = kadu_otr_new_fingerprint;
	Ops.write_fingerprints = kadu_otr_write_fingerprints;
	Ops.gone_secure = kadu_otr_gone_secure;
	Ops.gone_insecure = kadu_otr_gone_insecure;
	Ops.still_secure = kadu_otr_still_secure;
	Ops.max_message_size = kadu_otr_max_message_size;
	Ops.account_name = 0;
	Ops.account_name_free = 0;
	Ops.received_symkey = 0;
	Ops.otr_error_message = kadu_otr_otr_error_message;
	Ops.otr_error_message_free = kadu_otr_otr_error_message_free;
	Ops.resent_msg_prefix = kadu_otr_resent_msg_prefix;
	Ops.resent_msg_prefix_free = kadu_otr_resent_msg_prefix_free;
	Ops.handle_msg_event = kadu_otr_handle_msg_event;
	Ops.handle_smp_event = kadu_otr_handle_smp_event;
	Ops.create_instag = kadu_otr_create_instag;
	Ops.convert_msg = 0;
	Ops.convert_free = 0;
	Ops.timer_control = kadu_otr_timer_control;
}

OtrAppOpsWrapper::~OtrAppOpsWrapper()
{
}

void OtrAppOpsWrapper::setContextConverter(OtrContextConverter *contextConverter)
{
	ContextConverter = contextConverter;
}

void OtrAppOpsWrapper::setFingerprintService(OtrFingerprintService *fingerprintService)
{
	FingerprintService = fingerprintService;
}

void OtrAppOpsWrapper::setMessageManager(MessageManager *messageManager)
{
	CurrentMessageManager = messageManager;
}

void OtrAppOpsWrapper::setUserState(OtrUserState *userState)
{
	UserState = userState;
}

void OtrAppOpsWrapper::setTrustLevelService(OtrTrustLevelService *trustLevelService)
{
	TrustLevelService = trustLevelService;
}

const OtrlMessageAppOps * OtrAppOpsWrapper::ops() const
{
	return &Ops;
}

void OtrAppOpsWrapper::startPrivateConversation(const Contact &contact)
{
	if (!CurrentMessageManager)
		return;

	OtrTrustLevel::Level level = OtrTrustLevelContactStore::loadTrustLevelFromContact(contact);
	if (OtrTrustLevel::TRUST_PRIVATE == level)
		return;

	Account account = contact.contactAccount();
	OtrPolicy otrPolicy = OtrPolicyAccountStore::loadPolicyFromAccount(account);
	QString message = QString::fromUtf8(otrl_proto_default_query_msg(qPrintable(account.id()), otrPolicy.toOtrPolicy()));

	Chat chat = ChatTypeContact::findChat(contact, ActionCreateAndAdd);

	emit tryToStartSession(chat);
	CurrentMessageManager.data()->sendMessage(chat, message, true);
}

void OtrAppOpsWrapper::endPrivateConversation(const Contact &contact)
{
	if (!UserState)
		return;

	Chat chat = ChatTypeContact::findChat(contact, ActionCreateAndAdd);

	OtrOpData opData;
	opData.setAppOpsWrapper(this);
	opData.setChat(chat);
	opData.setPeerDisplay(contact.display(true));

	otrl_message_disconnect_all_instances(UserState->userState(), &Ops, &opData,
										  qPrintable(contact.contactAccount().id()),
										  qPrintable(contact.contactAccount().protocolName()),
										  qPrintable(contact.id()));

	emit goneInsecure(chat);
}

void OtrAppOpsWrapper::peerClosedSession(const Contact &contact)
{
	emit peerClosedSession(ChatTypeContact::findChat(contact, ActionCreateAndAdd));
}

OtrlPolicy OtrAppOpsWrapper::policy(OtrOpData *otrOpData) const
{
	Account account = otrOpData->chat().chatAccount();
	OtrPolicy policy = OtrPolicyAccountStore::loadPolicyFromAccount(account);

	return policy.toOtrPolicy();
}

void OtrAppOpsWrapper::createPrivateKey(OtrOpData *otrOpData) const
{
	Account account = otrOpData->chat().chatAccount();
	otrOpData->privateKeyService()->createPrivateKey(account);
}

OtrAppOpsWrapper::IsLoggedInStatus OtrAppOpsWrapper::isLoggedIn(OtrOpData *otrOpData, const QString &contactId) const
{
	Account account = otrOpData->chat().chatAccount();
	Contact contact = ContactManager::instance()->byId(account, contactId, ActionReturnNull);

	if (!contact)
		return NotSure;

	if (contact.currentStatus().isDisconnected())
		return NotLoggedIn;
	else
		return LoggedIn;
}

void OtrAppOpsWrapper::injectMessage(OtrOpData *otrOpData, const QByteArray &messageContent) const
{
	Chat chat = otrOpData->chat();
	MessageManager::instance()->sendRawMessage(chat, messageContent);
}

void OtrAppOpsWrapper::updateContextList(OtrOpData *otrOpData)
{
	Q_UNUSED(otrOpData);

	if (TrustLevelService)
		TrustLevelService.data()->updateTrustLevels();
}

void OtrAppOpsWrapper::writeFingerprints()
{
	if (FingerprintService)
		FingerprintService.data()->writeFingerprints();
}

void OtrAppOpsWrapper::goneSecure(OtrOpData *otrOpData) const
{
	emit goneSecure(otrOpData->chat());
}

void OtrAppOpsWrapper::goneInsecure(OtrOpData *otrOpData) const
{
	emit goneInsecure(otrOpData->chat());
}

void OtrAppOpsWrapper::stillSecure(OtrOpData *otrOpData) const
{
	emit stillSecure(otrOpData->chat());
}

int OtrAppOpsWrapper::maxMessageSize(OtrOpData *otrOpData) const
{
	Account account = otrOpData->chat().chatAccount();
	Protocol *protocolHandler = account.protocolHandler();
	if (!protocolHandler)
		return 0;
	ChatService *chatService = protocolHandler->chatService();
	if (!chatService)
		return 0;
	return chatService->maxMessageLength();
}

QString OtrAppOpsWrapper::errorMessage(OtrOpData *otrOpData, OtrlErrorCode errorCode) const
{
	Account account = otrOpData->chat().chatAccount();
	Contact receiver = otrOpData->chat().contacts().toContact();

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

void OtrAppOpsWrapper::handleMsgEvent(OtrOpData *otrOpData, OtrlMessageEvent event,
									  const QString &message, gcry_error_t errorCode) const
{
	QString peerDisplay = otrOpData->peerDisplay();
	QString errorMessage = messageString(event, message, errorCode, peerDisplay);

	if (errorMessage.isEmpty())
		return;

	ChatWidget *chatWidget = ChatWidgetManager::instance()->byChat(otrOpData->chat(), false);
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

void OtrAppOpsWrapper::createInstanceTag(OtrOpData *otrOpData)
{
	Q_ASSERT(UserState);

	Account account = otrOpData->chat().chatAccount();
	QString fileName = instanceTagsFileName();

	otrl_instag_generate(UserState->userState(), fileName.toUtf8().data(), account.id().toUtf8().data(), account.protocolName().toUtf8().data());
}

QString OtrAppOpsWrapper::instanceTagsFileName() const
{
	return KaduPaths::instance()->profilePath() + QString("/keys/otr_instance_tags");
}
