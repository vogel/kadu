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

#ifndef ENCRYPTION_NG_OTR_APP_OPS_WRAPPER_H
#define ENCRYPTION_NG_OTR_APP_OPS_WRAPPER_H

#include <QtCore/QObject>
#include <QtCore/QWeakPointer>

extern "C" {
#	include <libotr/proto.h>
#	include <libotr/message.h>
}

class QTimer;

class EncryptionNgOtrOpData;
class EncryptionNgOtrUserState;

class EncryptionNgOtrAppOpsWrapper : public QObject
{
	Q_OBJECT

	enum IsLoggedInStatus {
		NotSure = -1,
		NotLoggedIn = 0,
		LoggedIn = 1
	};

	friend OtrlPolicy kadu_enomf_policy(void *, ConnContext *);
	friend void kadu_enomf_create_privkey(void *, const char *, const char *);
	friend int kadu_enomf_is_logged_in(void *, const char *, const char *, const char *);
	friend void kadu_enomf_inject_message(void *, const char *, const char *, const char *, const char *);
	friend void kadu_enomf_gone_secure(void *, ConnContext *);
	friend void kadu_enomf_gone_insecure(void *, ConnContext *);
	friend void kadu_enomf_still_secure(void *, ConnContext *, int);
	friend int kadu_enomf_max_message_size(void *, ConnContext *);
	friend const char * kadu_enomf_otr_error_message(void *, ConnContext *, OtrlErrorCode);
	friend const char * kadu_enomf_resent_msg_prefix(void *, ConnContext *);
	friend void kadu_enomf_timer_control(void *, unsigned int);

	EncryptionNgOtrUserState *UserState;

	OtrlMessageAppOps Ops;
	QTimer *OtrTimer;

	OtrlPolicy policy(EncryptionNgOtrOpData *ngOtrOpData) const;
	void createPrivateKey(EncryptionNgOtrOpData *ngOtrOpData) const;
	IsLoggedInStatus isLoggedIn(EncryptionNgOtrOpData *ngOtrOpData, const QString &contactId) const;
	void injectMessage(EncryptionNgOtrOpData *ngOtrOpData, const QString &messageContent) const;
	void goneSecure(EncryptionNgOtrOpData *ngOtrOpData) const;
	void goneInsecure(EncryptionNgOtrOpData *ngOtrOpData) const;
	void stillSecure(EncryptionNgOtrOpData *ngOtrOpData) const;
	int maxMessageSize(EncryptionNgOtrOpData *ngOtrOpData) const;
	QString errorMessage(EncryptionNgOtrOpData *ngOtrOpData, OtrlErrorCode errorCode) const;
	QString resentMessagePrefix() const;
	void timerControl(int intervalSeconds);

private slots:
	void otrTimerTimeout();

public:
	explicit EncryptionNgOtrAppOpsWrapper();
	virtual ~EncryptionNgOtrAppOpsWrapper();

	void setUserState(EncryptionNgOtrUserState *userState);

	const OtrlMessageAppOps * ops() const;

};

#endif // ENCRYPTION_NG_OTR_APP_OPS_WRAPPER_H
