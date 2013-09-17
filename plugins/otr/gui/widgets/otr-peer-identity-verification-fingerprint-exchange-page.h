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

#ifndef OTR_PEER_IDENTITY_VERIFICATION_FINGERPRINT_EXCHANGE_PAGE_H
#define OTR_PEER_IDENTITY_VERIFICATION_FINGERPRINT_EXCHANGE_PAGE_H

#include <QtGui/QWizardPage>

#include "contacts/contact.h"

class OtrFingerprintExtractor;
class OtrFingerprintService;

class OtrPeerIdentityVerificationFingerprintExchangePage : public QWizardPage
{
	Q_OBJECT

	Contact MyContact;
	QWeakPointer<OtrFingerprintService> FingerprintService;

	void createGui(OtrFingerprintExtractor *fingerprintExtractor);

public:
	explicit OtrPeerIdentityVerificationFingerprintExchangePage(const Contact &contact, OtrFingerprintExtractor *fingerprintExtractor, QWidget *parent = 0);
	virtual ~OtrPeerIdentityVerificationFingerprintExchangePage();

	void setFingerprintTrust(OtrFingerprintService *fingerprintService);

	virtual bool validatePage();

};

#endif // OTR_PEER_IDENTITY_VERIFICATION_FINGERPRINT_EXCHANGE_PAGE_H
