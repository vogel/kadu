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

#include "otr-op-data.h"

OtrOpData::OtrOpData() :
		AppOpsWrapper(0), ErrorMessageService(0), FingerprintService(0), InstanceTagService(0),
		IsLoggedInService(0), MessageEventService(0), MessageService(0),
		PeerIdentityVerificationService(0), PolicyService(0), PrivateKeyService(0), SessionService(0),
		TimerService(0), TrustLevelService(0)
{
}

OtrOpData::OtrOpData(const OtrOpData &copyMe)
{
	*this = copyMe;
}


OtrOpData::~OtrOpData()
{
}

OtrOpData & OtrOpData::operator = (const OtrOpData &copyMe)
{
	AppOpsWrapper = copyMe.AppOpsWrapper;
	ErrorMessageService = copyMe.ErrorMessageService;
	FingerprintService = copyMe.FingerprintService;
	InstanceTagService = copyMe.InstanceTagService;
	IsLoggedInService = copyMe.IsLoggedInService;
	MessageEventService = copyMe.MessageEventService;
	MessageService = copyMe.MessageService;
	PeerIdentityVerificationService = copyMe.PeerIdentityVerificationService;
	PolicyService = copyMe.PolicyService;
	PrivateKeyService = copyMe.PrivateKeyService;
	SessionService = copyMe.SessionService;
	TimerService = copyMe.TimerService;
	TrustLevelService = copyMe.TrustLevelService;
	MyContact = copyMe.MyContact;
	PeerDisplay = copyMe.PeerDisplay;

	return *this;
}

void OtrOpData::setAppOpsWrapper(OtrAppOpsWrapper *appOpsWrapper)
{
	AppOpsWrapper = appOpsWrapper;
}

OtrAppOpsWrapper * OtrOpData::appOpsWrapper() const
{
	return AppOpsWrapper;
}

void OtrOpData::setErrorMessageService(OtrErrorMessageService *errorMessageService)
{
	ErrorMessageService = errorMessageService;
}

OtrErrorMessageService * OtrOpData::errorMessageService() const
{
	return ErrorMessageService;
}

void OtrOpData::setFingerprintService(OtrFingerprintService *fingerprintService)
{
	FingerprintService = fingerprintService;
}

OtrFingerprintService * OtrOpData::fingerprintService() const
{
	return FingerprintService;
}

void OtrOpData::setInstanceTagService(OtrInstanceTagService *instanceTagService)
{
	InstanceTagService = instanceTagService;
}

OtrInstanceTagService * OtrOpData::instanceTagService() const
{
	return InstanceTagService;
}

void OtrOpData::setIsLoggedInService(OtrIsLoggedInService *isLoggedInService)
{
	IsLoggedInService = isLoggedInService;
}

OtrIsLoggedInService * OtrOpData::isLoggedInService() const
{
	return IsLoggedInService;
}

void OtrOpData::setMessageEventService(OtrMessageEventService *messageEventService)
{
	MessageEventService = messageEventService;
}

OtrMessageEventService * OtrOpData::messageEventService() const
{
	return MessageEventService;
}

void OtrOpData::setMessageService(OtrMessageService *messageService)
{
	MessageService = messageService;
}

OtrMessageService * OtrOpData::messageService() const
{
	return MessageService;
}

void OtrOpData::setPeerIdentityVerificationService(OtrPeerIdentityVerificationService *peerIdentityVerificationService)
{
	PeerIdentityVerificationService = peerIdentityVerificationService;
}

OtrPeerIdentityVerificationService * OtrOpData::peerIdentityVerificationService() const
{
	return PeerIdentityVerificationService;
}

void OtrOpData::setPolicyService(OtrPolicyService *policyService)
{
	PolicyService = policyService;
}

OtrPolicyService * OtrOpData::policyService() const
{
	return PolicyService;
}

void OtrOpData::setPrivateKeyService(OtrPrivateKeyService *privateKeyService)
{
	PrivateKeyService = privateKeyService;
}

OtrPrivateKeyService * OtrOpData::privateKeyService() const
{
	return PrivateKeyService;
}

void OtrOpData::setSessionService(OtrSessionService *sessionService)
{
	SessionService = sessionService;
}

OtrSessionService * OtrOpData::sessionService() const
{
	return SessionService;
}

void OtrOpData::setTimerService(OtrTimerService *timerService)
{
	TimerService = timerService;
}

OtrTimerService * OtrOpData::timerService() const
{
	return TimerService;
}

void OtrOpData::setTrustLevelService(OtrTrustLevelService *trustLevelService)
{
	TrustLevelService = trustLevelService;
}

OtrTrustLevelService * OtrOpData::trustLevelService() const
{
	return TrustLevelService;
}

void OtrOpData::setContact(const Contact &contact)
{
	MyContact = contact;
}

Contact OtrOpData::contact() const
{
	return MyContact;
}

void OtrOpData::setPeerDisplay(const QString &peerDisplay)
{
	PeerDisplay = peerDisplay;
}

QString OtrOpData::peerDisplay() const
{
	return PeerDisplay;
}

