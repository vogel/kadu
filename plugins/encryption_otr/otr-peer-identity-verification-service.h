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

#pragma once

#include <QtCore/QMap>
#include <QtCore/QObject>
#include <QtCore/QPointer>
#include <injeqt/injeqt.h>

extern "C" {
#	include <libotr/proto.h>
#	include <libotr/message.h>
}

class Contact;

class OtrAppOpsService;
class OtrContextConverter;
class OtrOpDataFactory;
class OtrUserStateService;
class OtrPeerIdentityVerificationState;

class OtrPeerIdentityVerificationService : public QObject
{
	Q_OBJECT

public:
	static void wrapperHandleSmpEvent(void *data, OtrlSMPEvent smpEvent, ConnContext *context, unsigned short progressPercent, char *question);

	Q_INVOKABLE OtrPeerIdentityVerificationService();
	virtual ~OtrPeerIdentityVerificationService();

public slots:
	void updateContactState(const Contact &contact, const OtrPeerIdentityVerificationState &state);
	void startQuestionAndAnswerVerification(const Contact &contact, const QString &question, const QString &answer);
	void startSharedSecretVerficiation(const Contact &contact, const QString &sharedSecret);
	void respondVerification(const Contact &contact, const QString &answer);
	void cancelVerification(const Contact &contact);

signals:
	void questionAnswerRequested(const Contact &contact, const QString &question);
	void sharedSecretRequested(const Contact &contact);
	void contactStateUpdated(const Contact &contact, const OtrPeerIdentityVerificationState &state);

private slots:
	INJEQT_SETTER void setAppOpsService(OtrAppOpsService *appOpsService);
	INJEQT_SETTER void setContextConverter(OtrContextConverter *contextConverter);
	INJEQT_SETTER void setOpDataFactory(OtrOpDataFactory *opDataFactory);
	INJEQT_SETTER void setUserStateService(OtrUserStateService *userStateService);

private:
	QPointer<OtrAppOpsService> AppOpsService;
	QPointer<OtrContextConverter> ContextConverter;
	QPointer<OtrOpDataFactory> OpDataFactory;
	QPointer<OtrUserStateService> UserStateService;

	void handleSmpEvent(const Contact &contact, OtrlSMPEvent smpEvent, int progressPercent, const QString &question);

};
