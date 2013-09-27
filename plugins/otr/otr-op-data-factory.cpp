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

#include "otr-app-ops-wrapper.h"
#include "otr-fingerprint-service.h"
#include "otr-instance-tag-service.h"
#include "otr-op-data.h"
#include "otr-peer-identity-verification-service.h"
#include "otr-policy-service.h"
#include "otr-private-key-service.h"
#include "otr-session-service.h"
#include "otr-trust-level-service.h"

#include "otr-op-data-factory.h"

OtrOpDataFactory::OtrOpDataFactory(QObject *parent) :
		QObject(parent)
{
}

OtrOpDataFactory::~OtrOpDataFactory()
{
}

void OtrOpDataFactory::setAppOpsWrapper(OtrAppOpsWrapper *appOpsWrapper)
{
	AppOpsWrapper = appOpsWrapper;
}

void OtrOpDataFactory::setFingerprintService(OtrFingerprintService *fingerprintService)
{
	FingerprintService = fingerprintService;
}

void OtrOpDataFactory::setInstanceTagService(OtrInstanceTagService *instanceTagService)
{
	InstanceTagService = instanceTagService;
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

void OtrOpDataFactory::setTrustLevelService(OtrTrustLevelService *trustLevelService)
{
	TrustLevelService = trustLevelService;
}

OtrOpData OtrOpDataFactory::opDataForContact(const Contact &contact)
{
	OtrOpData result;
	result.setAppOpsWrapper(AppOpsWrapper.data());
	result.setContact(contact);
	result.setFingerprintService(FingerprintService.data());
	result.setInstanceTagService(InstanceTagService.data());
	result.setPeerDisplay(contact.display(true));
	result.setPeerIdentityVerificationService(PeerIdentityVerificationService.data());
	result.setPolicyService(PolicyService.data());
	result.setPrivateKeyService(PrivateKeyService.data());
	result.setSessionService(SessionService.data());
	result.setTrustLevelService(TrustLevelService.data());

	return result;
}
