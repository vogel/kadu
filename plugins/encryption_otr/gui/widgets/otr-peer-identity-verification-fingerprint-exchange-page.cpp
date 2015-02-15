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

#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QRadioButton>
#include <QtWidgets/QVBoxLayout>

#include "accounts/account.h"
#include "protocols/protocol-factory.h"
#include "protocols/protocol.h"

#include "otr-fingerprint-service.h"

#include "otr-peer-identity-verification-fingerprint-exchange-page.h"

OtrPeerIdentityVerificationFingerprintExchangePage::OtrPeerIdentityVerificationFingerprintExchangePage(const Contact &contact, QWidget *parent) :
		QWizardPage(parent), MyContact(contact)
{
	createGui();
}

OtrPeerIdentityVerificationFingerprintExchangePage::~OtrPeerIdentityVerificationFingerprintExchangePage()
{
}

void OtrPeerIdentityVerificationFingerprintExchangePage::createGui()
{
	setTitle(tr("Fingerprint Exchange"));

	QVBoxLayout *layout = new QVBoxLayout(this);

	layout->addWidget(new QLabel(tr("Your Key Fingerprint (%1: %2):")
			.arg(MyContact.contactAccount().protocolHandler()->protocolFactory()->displayName())
			.arg(MyContact.contactAccount().id())));

	OwnFingerprint = new QLineEdit();
	OwnFingerprint->setReadOnly(true);
	layout->addWidget(OwnFingerprint);

	layout->addWidget(new QLabel(tr("<b>%1</b> Key Fingerprint (%2: %3):")
			.arg(MyContact.display(true))
			.arg(MyContact.contactAccount().protocolHandler()->protocolFactory()->displayName())
			.arg(MyContact.id())));

	PeerFingerprint = new QLineEdit();
	PeerFingerprint->setReadOnly(true);
	layout->addWidget(PeerFingerprint);

	FingerprintExchangeNotConfirm = new QRadioButton(tr("I'm not sure if above Key Fingerprint belongs to %1").arg(MyContact.display(true)));
	FingerprintExchangeConfirm = new QRadioButton(tr("I confirm that above Key Fingerprint belongs to %1").arg(MyContact.display(true)));
	registerField("fingerprintExchangeNotConfirm", FingerprintExchangeNotConfirm);
	registerField("fingerprintExchangeConfirm", FingerprintExchangeConfirm);

	layout->addWidget(FingerprintExchangeNotConfirm);
	layout->addWidget(FingerprintExchangeConfirm);
}

void OtrPeerIdentityVerificationFingerprintExchangePage::setFingerprintService(OtrFingerprintService *fingerprintService)
{
	FingerprintService = fingerprintService;
}

int OtrPeerIdentityVerificationFingerprintExchangePage::nextId() const
{
	return -1;
}

void OtrPeerIdentityVerificationFingerprintExchangePage::initializePage()
{
	if (!FingerprintService)
	{
		FingerprintExchangeConfirm->setEnabled(false);
		FingerprintExchangeNotConfirm->setChecked(true);
		return;
	}

	OwnFingerprint->setText(FingerprintService.data()->extractAccountFingerprint(MyContact.contactAccount()));
	PeerFingerprint->setText(FingerprintService.data()->extractContactFingerprint(MyContact));

	if (OtrFingerprintService::TrustVerified == FingerprintService.data()->contactFingerprintTrust(MyContact))
		FingerprintExchangeConfirm->setChecked(true);
	else
		FingerprintExchangeNotConfirm->setChecked(true);
}

bool OtrPeerIdentityVerificationFingerprintExchangePage::validatePage()
{
	if (!FingerprintService)
		return true;

	if (field("fingerprintExchangeConfirm").toBool())
		FingerprintService.data()->setContactFingerprintTrust(MyContact, OtrFingerprintService::TrustVerified);
	else
		FingerprintService.data()->setContactFingerprintTrust(MyContact, OtrFingerprintService::TrustNotVerified);

	return true;
}
