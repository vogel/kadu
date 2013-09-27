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

#ifndef OTR_PEER_IDENTITY_VERIFICATION_SERVICE_H
#define OTR_PEER_IDENTITY_VERIFICATION_SERVICE_H

#include <QtCore/QMap>
#include <QtCore/QObject>
#include <QtCore/QWeakPointer>

extern "C" {
#	include <libotr/proto.h>
#	include <libotr/message.h>
}

class Contact;

class OtrAppOpsWrapper;
class OtrContextConverter;
class OtrOpDataFactory;
class OtrUserStateService;
class OtrPeerIdentityVerificationState;

class OtrPeerIdentityVerificationService : public QObject
{
	Q_OBJECT

	QWeakPointer<OtrAppOpsWrapper> AppOpsWrapper;
	QWeakPointer<OtrContextConverter> ContextConverter;
	QWeakPointer<OtrOpDataFactory> OpDataFactory;
	QWeakPointer<OtrUserStateService> UserStateService;

	void handleSmpEvent(const Contact &contact, OtrlSMPEvent smpEvent, int progressPercent, const QString &question);

public:
	static void wrapperHandleSmpEvent(void *data, OtrlSMPEvent smpEvent, ConnContext *context, unsigned short progressPercent, char *question);

	explicit OtrPeerIdentityVerificationService(QObject *parent = 0);
	virtual ~OtrPeerIdentityVerificationService();

	void setAppOpsWrapper(OtrAppOpsWrapper *appOpsWrapper);
	void setContextConverter(OtrContextConverter *contextConverter);
	void setOpDataFactory(OtrOpDataFactory *opDataFactory);
	void setUserStateService(OtrUserStateService *userStateService);

public slots:
	void updateContactState(const Contact &contact, const OtrPeerIdentityVerificationState &state);
	void startQuestionAndAnswerVerification(const Contact &contact, const QString &question, const QString &answer);
	void startSharedSecretVerficiation(const Contact &contact, const QString &sharedSecret);
	void cancelVerification(const Contact &contact);

signals:
	void contactStateUpdated(const Contact &contact, const OtrPeerIdentityVerificationState &state);

};

#endif // OTR_PEER_IDENTITY_VERIFICATION_SERVICE_H
