/*
 * %kadu copyright begin%
 * Copyright 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2009, 2010 Tomasz Rostański (rozteck@interia.pl)
 * Copyright 2011, 2013, 2014 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2010, 2014 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#include <QtCore/QObject>
#include <QtCore/QPointer>
#include <injeqt/injeqt.h>

class Contact;

class OtrErrorMessageService;
class OtrFingerprintService;
class OtrInstanceTagService;
class OtrIsLoggedInService;
class OtrMessageEventService;
class OtrMessageService;
class OtrOpData;
class OtrPeerIdentityVerificationService;
class OtrPolicyService;
class OtrPrivateKeyService;
class OtrSessionService;
class OtrTimerService;
class OtrTrustLevelService;

class OtrOpDataFactory : public QObject
{
	Q_OBJECT

public:
	Q_INVOKABLE OtrOpDataFactory();
	virtual ~OtrOpDataFactory();

	OtrOpData opData();
	OtrOpData opDataForContact(const Contact &contact);

private slots:
	INJEQT_SETTER void setErrorMessageService(OtrErrorMessageService *errorMessageService);
	INJEQT_SETTER void setFingerprintService(OtrFingerprintService *fingerprintService);
	INJEQT_SETTER void setInstanceTagService(OtrInstanceTagService *instanceTagService);
	INJEQT_SETTER void setIsLoggedInService(OtrIsLoggedInService *isLoggedInService);
	INJEQT_SETTER void setMessageEventService(OtrMessageEventService *messageEventService);
	INJEQT_SETTER void setMessageService(OtrMessageService *messageService);
	INJEQT_SETTER void setPeerIdentityVerificationService(OtrPeerIdentityVerificationService *peerIdentityVerificationService);
	INJEQT_SETTER void setPolicyService(OtrPolicyService *policyService);
	INJEQT_SETTER void setPrivateKeyService(OtrPrivateKeyService *privateKeyService);
	INJEQT_SETTER void setSessionService(OtrSessionService *sessionService);
	INJEQT_SETTER void setTimerService(OtrTimerService *timerService);
	INJEQT_SETTER void setTrustLevelService(OtrTrustLevelService *trustLevelService);

private:
	QPointer<OtrErrorMessageService> ErrorMessageService;
	QPointer<OtrFingerprintService> FingerprintService;
	QPointer<OtrInstanceTagService> InstanceTagService;
	QPointer<OtrIsLoggedInService> IsLoggedInService;
	QPointer<OtrMessageEventService> MessageEventService;
	QPointer<OtrMessageService> MessageService;
	QPointer<OtrPeerIdentityVerificationService> PeerIdentityVerificationService;
	QPointer<OtrPolicyService> PolicyService;
	QPointer<OtrPrivateKeyService> PrivateKeyService;
	QPointer<OtrSessionService> SessionService;
	QPointer<OtrTimerService> TimerService;
	QPointer<OtrTrustLevelService> TrustLevelService;

};
