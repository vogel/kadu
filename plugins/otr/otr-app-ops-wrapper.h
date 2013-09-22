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

#ifndef OTR_APP_OPS_WRAPPER_H
#define OTR_APP_OPS_WRAPPER_H

#include <QtCore/QObject>
#include <QtCore/QWeakPointer>

#include "chat/chat.h"

extern "C" {
#	include <libotr/proto.h>
#	include <libotr/message.h>
}

class Contact;
class MessageManager;

class OtrContextConverter;
class OtrFingerprintService;
class OtrOpData;
class OtrTrustLevelService;
class OtrUserState;

class OtrAppOpsWrapper : public QObject
{
	Q_OBJECT

	enum IsLoggedInStatus {
		NotSure = -1,
		NotLoggedIn = 0,
		LoggedIn = 1
	};

	friend OtrlPolicy kadu_otr_policy(void *, ConnContext *);
	friend void kadu_otr_create_privkey(void *, const char *, const char *);
	friend int kadu_otr_is_logged_in(void *, const char *, const char *, const char *);
	friend void kadu_otr_inject_message(void *, const char *, const char *, const char *, const char *);
	friend void kadu_otr_update_context_list(void *);
	friend void kadu_otr_write_fingerprints(void *);
	friend void kadu_otr_gone_secure(void *, ConnContext *);
	friend void kadu_otr_gone_insecure(void *, ConnContext *);
	friend void kadu_otr_still_secure(void *, ConnContext *, int);
	friend int kadu_otr_max_message_size(void *, ConnContext *);
	friend const char * kadu_otr_otr_error_message(void *, ConnContext *, OtrlErrorCode);
	friend const char * kadu_otr_resent_msg_prefix(void *, ConnContext *);
	friend void kadu_otr_handle_msg_event(void *, OtrlMessageEvent, ConnContext *, const char *, gcry_error_t);
	friend void kadu_otr_handle_smp_event(void *, OtrlSMPEvent, ConnContext *, unsigned short, char *);
	friend void kadu_otr_create_instag(void *, const char *, const char *);

	QWeakPointer<OtrContextConverter> ContextConverter;
	QWeakPointer<OtrFingerprintService> FingerprintService;
	QWeakPointer<MessageManager> CurrentMessageManager;
	QWeakPointer<OtrTrustLevelService> TrustLevelService;
	OtrUserState *UserState;
	OtrlMessageAppOps Ops;

	OtrlPolicy policy(OtrOpData *ngOtrOpData) const;
	void createPrivateKey(OtrOpData *ngOtrOpData) const;
	IsLoggedInStatus isLoggedIn(OtrOpData *ngOtrOpData, const QString &contactId) const;
	void injectMessage(OtrOpData *ngOtrOpData, const QByteArray &messageContent) const;
	void updateContextList(OtrOpData *ngOtrOpData);
	void writeFingerprints();
	void goneSecure(OtrOpData *ngOtrOpData) const;
	void goneInsecure(OtrOpData *ngOtrOpData) const;
	void stillSecure(OtrOpData *ngOtrOpData) const;
	int maxMessageSize(OtrOpData *ngOtrOpData) const;
	QString errorMessage(OtrOpData *ngOtrOpData, OtrlErrorCode errorCode) const;
	QString resentMessagePrefix() const;
	void handleMsgEvent(OtrOpData *ngOtrOpData, OtrlMessageEvent event, const QString &message, gcry_error_t errorCode) const;
	void handleSmpEvent(OtrOpData *ngOtrOpData, OtrlSMPEvent event, unsigned short progressPercent, const QString &question);
	QString messageString(OtrlMessageEvent event, const QString &message, gcry_error_t errorCode, const QString &peerDisplay) const;
	QString gpgErrorString(gcry_error_t errorCode) const;
	void createInstanceTag(OtrOpData *ngOtrOpData);
	QString instanceTagsFileName() const;

public:
	explicit OtrAppOpsWrapper();
	virtual ~OtrAppOpsWrapper();

	void setContextConverter(OtrContextConverter *contextConverter);
	void setFingerprintService(OtrFingerprintService *fingerprintService);
	void setMessageManager(MessageManager *messageManager);
	void setUserState(OtrUserState *userState);
	void setTrustLevelService(OtrTrustLevelService *trustLevelService);

	const OtrlMessageAppOps * ops() const;

public slots:
	void startPrivateConversation(const Contact &contact);
	void endPrivateConversation(const Contact &contact);
	void peerClosedSession(const Contact &contact);

	void startSMPAskQuestion(const Contact &contact, const QString &question, const QString &answer);

signals:
	void tryToStartSession(const Chat &chat) const;
	void peerClosedSession(const Chat &chat) const;
	void goneSecure(const Chat &chat) const;
	void goneInsecure(const Chat &chat) const;
	void stillSecure(const Chat &chat) const;

};

#endif // OTR_APP_OPS_WRAPPER_H
