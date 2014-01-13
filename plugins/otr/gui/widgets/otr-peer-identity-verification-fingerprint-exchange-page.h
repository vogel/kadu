/*
 * %kadu copyright begin%
 * Copyright 2004 Adrian Smarzewski (adrian@kadu.net)
 * Copyright 2007, 2008, 2009, 2010, 2011, 2013 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2004, 2006 Marcin Ślusarz (joi@kadu.net)
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

#include <QtCore/QPointer>
#include <QtGui/QWizardPage>

#include "contacts/contact.h"

class QLineEdit;
class QRadioButton;

class OtrFingerprintService;

class OtrPeerIdentityVerificationFingerprintExchangePage : public QWizardPage
{
	Q_OBJECT

	Contact MyContact;
	QPointer<OtrFingerprintService> FingerprintService;

	QLineEdit *OwnFingerprint;
	QLineEdit *PeerFingerprint;
	QRadioButton *FingerprintExchangeNotConfirm;
	QRadioButton *FingerprintExchangeConfirm;

	void createGui();

public:
	explicit OtrPeerIdentityVerificationFingerprintExchangePage(const Contact &contact, QWidget *parent = 0);
	virtual ~OtrPeerIdentityVerificationFingerprintExchangePage();

	void setFingerprintService(OtrFingerprintService *fingerprintService);

	virtual int nextId() const;
	virtual void initializePage();
	virtual bool validatePage();

};

#endif // OTR_PEER_IDENTITY_VERIFICATION_FINGERPRINT_EXCHANGE_PAGE_H
