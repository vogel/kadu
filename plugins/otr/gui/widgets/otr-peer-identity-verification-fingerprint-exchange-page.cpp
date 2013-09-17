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

#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QRadioButton>
#include <QtGui/QVBoxLayout>

#include "accounts/account.h"
#include "protocols/protocol.h"
#include "protocols/protocol-factory.h"

#include "otr-fingerprint-extractor.h"
#include "otr-fingerprint-trust.h"

#include "otr-peer-identity-verification-fingerprint-exchange-page.h"

OtrPeerIdentityVerificationFingerprintExchangePage::OtrPeerIdentityVerificationFingerprintExchangePage(const Contact &contact, OtrFingerprintExtractor *fingerprintExtractor, QWidget *parent) :
		QWizardPage(parent), MyContact(contact)
{
	setTitle(tr("Select Verification Method"));

	createGui(fingerprintExtractor);
}

OtrPeerIdentityVerificationFingerprintExchangePage::~OtrPeerIdentityVerificationFingerprintExchangePage()
{
}

void OtrPeerIdentityVerificationFingerprintExchangePage::setFingerprintTrust(OtrFingerprintTrust *fingerprintTrust)
{
	FingerprintTrust = fingerprintTrust;
}

void OtrPeerIdentityVerificationFingerprintExchangePage::createGui(OtrFingerprintExtractor *fingerprintExtractor)
{
	setTitle(tr("Fingerprint Exchange"));

	QVBoxLayout *layout = new QVBoxLayout(this);

	layout->addSpacing(8);
	layout->addWidget(new QLabel(tr("Your Key Fingerprint (%1: %2): ")
			.arg(MyContact.contactAccount().protocolHandler()->protocolFactory()->displayName())
			.arg(MyContact.contactAccount().id())));

	QLineEdit *ownFingerprint = new QLineEdit(fingerprintExtractor->extractAccountFingerprint(MyContact.contactAccount()));
	ownFingerprint->setReadOnly(true);
	layout->addWidget(ownFingerprint);

	layout->addWidget(new QLabel(tr("<b>%1</b> Key Fingerprint (%2: %3): ")
			.arg(MyContact.display(true))
			.arg(MyContact.contactAccount().protocolHandler()->protocolFactory()->displayName())
			.arg(MyContact.id())));

	QLineEdit *peerFingerprint = new QLineEdit(fingerprintExtractor->extractContactFingerprint(MyContact));
	peerFingerprint->setReadOnly(true);
	layout->addWidget(peerFingerprint);

	QRadioButton *fingerprintExchangeNotConfirm = new QRadioButton(tr("I'm not sure if above Key Fingerprint belongs to %1").arg(MyContact.display(true)));
	QRadioButton *fingerprintExchangeConfirm = new QRadioButton(tr("I confirm that above Key Fingerprint belongs to %1").arg(MyContact.display(true)));
	registerField("fingerprintExchangeNotConfirm", fingerprintExchangeNotConfirm);
	registerField("fingerprintExchangeConfirm", fingerprintExchangeConfirm);

	if (FingerprintTrust && OtrFingerprintTrust::TrustVerified == FingerprintTrust.data()->contactFingerprintTrust(MyContact))
		fingerprintExchangeConfirm->setChecked(true);
	else
		fingerprintExchangeNotConfirm->setChecked(true);

	layout->addSpacing(8);
	layout->addWidget(fingerprintExchangeNotConfirm);
	layout->addWidget(fingerprintExchangeConfirm);
}

bool OtrPeerIdentityVerificationFingerprintExchangePage::validatePage()
{
	if (!FingerprintTrust)
		return true;

	if (field("fingerprintExchangeConfirm").toBool())
		FingerprintTrust.data()->setContactFingerprintTrust(MyContact, OtrFingerprintTrust::TrustVerified);
	else
		FingerprintTrust.data()->setContactFingerprintTrust(MyContact, OtrFingerprintTrust::TrustNotVerified);

	return true;
}
