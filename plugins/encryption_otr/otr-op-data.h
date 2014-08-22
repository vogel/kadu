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

#ifndef OTR_OP_DATA_H
#define OTR_OP_DATA_H

#include <QtCore/QPointer>

#include "contacts/contact.h"

class OtrErrorMessageService;
class OtrFingerprintService;
class OtrInstanceTagService;
class OtrIsLoggedInService;
class OtrMessageEventService;
class OtrMessageService;
class OtrPeerIdentityVerificationService;
class OtrPolicyService;
class OtrPrivateKeyService;
class OtrSessionService;
class OtrTimerService;
class OtrTrustLevelService;

class OtrOpData
{
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
	Contact MyContact;
	QString PeerDisplay;

public:
	explicit OtrOpData();
	OtrOpData(const OtrOpData &copyMe);
	virtual ~OtrOpData();

	OtrOpData & operator = (const OtrOpData &copyMe);

	void setErrorMessageService(OtrErrorMessageService *errorMessageService);
	OtrErrorMessageService * errorMessageService() const;

	void setFingerprintService(OtrFingerprintService *fingerprintService);
	OtrFingerprintService * fingerprintService() const;

	void setInstanceTagService(OtrInstanceTagService *instanceTagService);
	OtrInstanceTagService * instanceTagService() const;

	void setIsLoggedInService(OtrIsLoggedInService *isLoggedInService);
	OtrIsLoggedInService * isLoggedInService() const;

	void setMessageEventService(OtrMessageEventService *messageEventService);
	OtrMessageEventService * messageEventService() const;

	void setMessageService(OtrMessageService *messageService);
	OtrMessageService * messageService() const;

	void setPeerIdentityVerificationService(OtrPeerIdentityVerificationService *peerIdentityVerificationService);
	OtrPeerIdentityVerificationService * peerIdentityVerificationService() const;

	void setPolicyService(OtrPolicyService *policyService);
	OtrPolicyService * policyService() const;

	void setPrivateKeyService(OtrPrivateKeyService *privateKeyService);
	OtrPrivateKeyService * privateKeyService() const;

	void setSessionService(OtrSessionService *sessionService);
	OtrSessionService * sessionService() const;

	void setTimerService(OtrTimerService *timerService);
	OtrTimerService * timerService() const;

	void setTrustLevelService(OtrTrustLevelService *trustLevelService);
	OtrTrustLevelService * trustLevelService() const;

	void setContact(const Contact &contact);
	Contact contact() const;

	void setPeerDisplay(const QString &peerDisplay);
	QString peerDisplay() const;

};

#endif // OTR_OP_DATA_H
