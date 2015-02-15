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
#include <QtWidgets/QVBoxLayout>

#include "accounts/account.h"
#include "protocols/protocol-factory.h"
#include "protocols/protocol.h"

#include "gui/windows/otr-peer-identity-verification-window.h"
#include "otr-fingerprint-service.h"
#include "otr-peer-identity-verification-service.h"

#include "otr-peer-identity-verification-respond-shared-secret-page.h"

OtrPeerIdentityVerificationRespondSharedSecretPage::OtrPeerIdentityVerificationRespondSharedSecretPage(const Contact &contact, QWidget *parent) :
		QWizardPage(parent), MyContact(contact)
{
	createGui();
}

OtrPeerIdentityVerificationRespondSharedSecretPage::~OtrPeerIdentityVerificationRespondSharedSecretPage()
{
}

void OtrPeerIdentityVerificationRespondSharedSecretPage::createGui()
{
	setButtonText(QWizard::CommitButton, tr("Send Shared Secret"));
	setCommitPage(true);
	setTitle(tr("Respond to Shared Secret"));

	QVBoxLayout *layout = new QVBoxLayout(this);

	QLineEdit *sharedSecretEdit = new QLineEdit();

	layout->addWidget(new QLabel(tr("%1 wants to verify your identity using Shared Secret method.").arg(MyContact.display(true))));
	layout->addWidget(new QLabel(tr("Shared Secret that is known only for you and %1:").arg(MyContact.display(true))));
	layout->addWidget(sharedSecretEdit);

	registerField("respondSharedSecret*", sharedSecretEdit);
}

void OtrPeerIdentityVerificationRespondSharedSecretPage::setPeerIdentityVerificationService(OtrPeerIdentityVerificationService *peerIdentityVerificationService)
{
	PeerIdentityVerificationService = peerIdentityVerificationService;
}

void OtrPeerIdentityVerificationRespondSharedSecretPage::rejected()
{
	if (!PeerIdentityVerificationService)
		return;

	if (wizard()->currentPage() == this)
		PeerIdentityVerificationService.data()->cancelVerification(MyContact);
}

int OtrPeerIdentityVerificationRespondSharedSecretPage::nextId() const
{
	return OtrPeerIdentityVerificationWindow::ProgressPage;
}

void OtrPeerIdentityVerificationRespondSharedSecretPage::initializePage()
{
	setField("respondSharedSecret", QString());
}

bool OtrPeerIdentityVerificationRespondSharedSecretPage::validatePage()
{
	QString respondSharedSecret = field("respondSharedSecret").toString();

	if (respondSharedSecret.isEmpty())
		return false;

	if (PeerIdentityVerificationService)
		PeerIdentityVerificationService.data()->respondVerification(MyContact, field("respondSharedSecret").toString());

	return true;
}
