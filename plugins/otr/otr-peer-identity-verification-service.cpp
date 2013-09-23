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

#include "contacts/contact.h"

#include "otr-app-ops-wrapper.h"
#include "otr-peer-identity-verification-state.h"

#include "otr-peer-identity-verification-service.h"

OtrPeerIdentityVerificationService::OtrPeerIdentityVerificationService(QObject *parent) :
		QObject(parent)
{
}

OtrPeerIdentityVerificationService::~OtrPeerIdentityVerificationService()
{
}

void OtrPeerIdentityVerificationService::setAppOpsWrapper(OtrAppOpsWrapper *otrAppOpsWrapper)
{
	AppOpsWrapper = otrAppOpsWrapper;
}

void OtrPeerIdentityVerificationService::updateContactState(const Contact &contact, const OtrPeerIdentityVerificationState &state)
{
	if (OtrPeerIdentityVerificationState::StateFailed == state.state())
		cancelVerification(contact);

	emit contactStateUpdated(contact, state);
}

void OtrPeerIdentityVerificationService::cancelVerification(const Contact &contact)
{
	if (AppOpsWrapper)
		AppOpsWrapper.data()->abortSMP(contact);
}
