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

#ifndef OTR_PEER_IDENTITY_VERIFICATION_RESULT_PAGE_H
#define OTR_PEER_IDENTITY_VERIFICATION_RESULT_PAGE_H

#include <QtWidgets/QWizardPage>

#include "contacts/contact.h"

#include "otr-trust-level-service.h"

#include "otr-peer-identity-verification-state.h"

class OtrPeerIdentityVerificationResultPage : public QWizardPage
{
	Q_OBJECT

	QPointer<OtrTrustLevelService> TrustLevelService;

	Contact MyContact;

	void createGui();
	QString stateToString(const OtrPeerIdentityVerificationState::State &state, OtrTrustLevelService::TrustLevel trustLevel);

public:
	explicit OtrPeerIdentityVerificationResultPage(const Contact &contact, QWidget *parent = 0);
	virtual ~OtrPeerIdentityVerificationResultPage();

	void setTrustLevelService(OtrTrustLevelService *trustLevelService);

	virtual int nextId() const;
	virtual void initializePage();

};

#endif // OTR_PEER_IDENTITY_VERIFICATION_RESULT_PAGE_H
