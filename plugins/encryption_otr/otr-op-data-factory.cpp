/*
 * %kadu copyright begin%
 * Copyright 2013, 2014 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "otr-error-message-service.h"
#include "otr-fingerprint-service.h"
#include "otr-instance-tag-service.h"
#include "otr-is-logged-in-service.h"
#include "otr-message-event-service.h"
#include "otr-message-service.h"
#include "otr-op-data.h"
#include "otr-peer-identity-verification-service.h"
#include "otr-policy-service.h"
#include "otr-private-key-service.h"
#include "otr-session-service.h"
#include "otr-timer-service.h"
#include "otr-trust-level-service.h"

#include "otr-op-data-factory.h"

OtrOpDataFactory::OtrOpDataFactory(QObject *parent) :
		QObject(parent)
{
}

OtrOpDataFactory::~OtrOpDataFactory()
{
}

void OtrOpDataFactory::setErrorMessageService(OtrErrorMessageService *errorMessageService)
{
	ErrorMessageService = errorMessageService;
}

void OtrOpDataFactory::setFingerprintService(OtrFingerprintService *fingerprintService)
{
	FingerprintService = fingerprintService;
}

void OtrOpDataFactory::setInstanceTagService(OtrInstanceTagService *instanceTagService)
{
	InstanceTagService = instanceTagService;
}

void OtrOpDataFactory::setIsLoggedInService(OtrIsLoggedInService *isLoggedInService)
{
	IsLoggedInService = isLoggedInService;
}

void OtrOpDataFactory::setMessageEventService(OtrMessageEventService *messageEventService)
{
	MessageEventService = messageEventService;
}

void OtrOpDataFactory::setMessageService(OtrMessageService *messageService)
{
	MessageService = messageService;
}

void OtrOpDataFactory::setPeerIdentityVerificationService(OtrPeerIdentityVerificationService *peerIdentityVerificationService)
{
	PeerIdentityVerificationService = peerIdentityVerificationService;
}

void OtrOpDataFactory::setPolicyService(OtrPolicyService *policyService)
{
	PolicyService = policyService;
}

void OtrOpDataFactory::setPrivateKeyService(OtrPrivateKeyService *privateKeyService)
{
	PrivateKeyService = privateKeyService;
}

void OtrOpDataFactory::setSessionService(OtrSessionService *sessionService)
{
	SessionService = sessionService;
}

void OtrOpDataFactory::setTimerService(OtrTimerService *timerService)
{
	TimerService = timerService;
}

void OtrOpDataFactory::setTrustLevelService(OtrTrustLevelService *trustLevelService)
{
	TrustLevelService = trustLevelService;
}

OtrOpData OtrOpDataFactory::opData()
{
	OtrOpData result;
	result.setErrorMessageService(ErrorMessageService.data());
	result.setFingerprintService(FingerprintService.data());
	result.setInstanceTagService(InstanceTagService.data());
	result.setIsLoggedInService(IsLoggedInService.data());
	result.setMessageEventService(MessageEventService.data());
	result.setMessageService(MessageService.data());
	result.setPeerIdentityVerificationService(PeerIdentityVerificationService.data());
	result.setPolicyService(PolicyService.data());
	result.setPrivateKeyService(PrivateKeyService.data());
	result.setSessionService(SessionService.data());
	result.setTimerService(TimerService.data());
	result.setTrustLevelService(TrustLevelService.data());

	return result;
}

OtrOpData OtrOpDataFactory::opDataForContact(const Contact &contact)
{
	OtrOpData result = opData();
	result.setContact(contact);
	result.setPeerDisplay(contact.display(true));

	return result;
}
