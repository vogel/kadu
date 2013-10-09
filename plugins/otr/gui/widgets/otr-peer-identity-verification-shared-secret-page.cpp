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
#include <QtGui/QVBoxLayout>

#include "accounts/account.h"
#include "protocols/protocol-factory.h"
#include "protocols/protocol.h"

#include "gui/windows/otr-peer-identity-verification-window.h"
#include "otr-fingerprint-service.h"
#include "otr-peer-identity-verification-service.h"

#include "otr-peer-identity-verification-shared-secret-page.h"

OtrPeerIdentityVerificationSharedSecretPage::OtrPeerIdentityVerificationSharedSecretPage(const Contact &contact, QWidget *parent) :
		QWizardPage(parent), MyContact(contact)
{
	createGui();
}

OtrPeerIdentityVerificationSharedSecretPage::~OtrPeerIdentityVerificationSharedSecretPage()
{
}

void OtrPeerIdentityVerificationSharedSecretPage::createGui()
{
	setButtonText(QWizard::CommitButton, tr("Ask for Shared Secret"));
	setCommitPage(true);
	setTitle(tr("Shared Secret"));

	QVBoxLayout *layout = new QVBoxLayout(this);

	QLineEdit *sharedSecretEdit = new QLineEdit();

	layout->addWidget(new QLabel(tr("Shared Secret that is known only for you and %1:").arg(MyContact.display(true))));
	layout->addWidget(sharedSecretEdit);

	registerField("sharedSecret*", sharedSecretEdit);
}

void OtrPeerIdentityVerificationSharedSecretPage::setPeerIdentityVerificationService(OtrPeerIdentityVerificationService *peerIdentityVerificationService)
{
	PeerIdentityVerificationService = peerIdentityVerificationService;
}

int OtrPeerIdentityVerificationSharedSecretPage::nextId() const
{
	return OtrPeerIdentityVerificationWindow::ProgressPage;
}

void OtrPeerIdentityVerificationSharedSecretPage::initializePage()
{
	setField("sharedSecret", QString());
}

bool OtrPeerIdentityVerificationSharedSecretPage::validatePage()
{
	QString sharedSecret = field("sharedSecret").toString();

	if (sharedSecret.isEmpty())
		return false;

	if (PeerIdentityVerificationService)
		PeerIdentityVerificationService.data()->startSharedSecretVerficiation(MyContact, field("sharedSecret").toString());

	return true;
}
