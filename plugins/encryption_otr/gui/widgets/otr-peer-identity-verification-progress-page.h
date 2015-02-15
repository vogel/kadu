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

#ifndef OTR_PEER_IDENTITY_VERIFICATION_PROGRESS_PAGE_H
#define OTR_PEER_IDENTITY_VERIFICATION_PROGRESS_PAGE_H

#include <QtWidgets/QWizardPage>

#include "contacts/contact.h"

#include "otr-peer-identity-verification-state.h"

class OtrPeerIdentityVerificationService;

class OtrPeerIdentityVerificationProgressPage : public QWizardPage
{
	Q_OBJECT

	QPointer<OtrPeerIdentityVerificationService> PeerIdentityVerificationService;

	Contact MyContact;
	OtrPeerIdentityVerificationState State;

	void createGui();
	void setState(const OtrPeerIdentityVerificationState &state);
	QString stateToString(const OtrPeerIdentityVerificationState &state);
	void cancelVerification();
	bool canCancelVerification() const;

public:
	explicit OtrPeerIdentityVerificationProgressPage(const Contact &contact, QWidget *parent = 0);
	virtual ~OtrPeerIdentityVerificationProgressPage();

	void setPeerIdentityVerificationService(OtrPeerIdentityVerificationService *peerIdentityVerificationService);

	virtual int nextId() const;
	virtual void initializePage();
	virtual bool validatePage();

public slots:
	void rejected();
	void updateContactState(const Contact &contact, const OtrPeerIdentityVerificationState &state);

signals:
	void finished();

};

#endif // OTR_PEER_IDENTITY_VERIFICATION_PROGRESS_PAGE_H
