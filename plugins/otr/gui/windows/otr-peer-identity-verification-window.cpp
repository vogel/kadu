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

#include "gui/widgets/otr-peer-identity-verification-fingerprint-exchange-page.h"
#include "gui/widgets/otr-peer-identity-verification-select-method-page.h"

#include "otr-peer-identity-verification-window.h"

OtrPeerIdentityVerificationWindow::OtrPeerIdentityVerificationWindow(const Contact &contact,
																	 OtrFingerprintService *fingerprintService, QWidget *parent) :
		QWizard(parent), MyContact(contact)
{
	setAttribute(Qt::WA_DeleteOnClose, true);
	setWindowTitle(tr("Verify Identity of %1").arg(MyContact.display(true)));

	createGui(fingerprintService);
}

OtrPeerIdentityVerificationWindow::~OtrPeerIdentityVerificationWindow()
{
	emit destroyed(MyContact);
}

void OtrPeerIdentityVerificationWindow::createGui(OtrFingerprintService *fingerprintService)
{
	setPage(SelectMethodPage, new OtrPeerIdentityVerificationSelectMethodPage(this));
	setPage(QuestionAndAnswerPage, createQuestionAndAnswerPage());
	setPage(SharedSecretPage, createSharedSecretPage());

	if (fingerprintService)
	{
		OtrPeerIdentityVerificationFingerprintExchangePage *fingerprintExchangePage = new OtrPeerIdentityVerificationFingerprintExchangePage(MyContact, this);
		fingerprintExchangePage->setFingerprintService(fingerprintService);
		setPage(FingerprintExchangePage, fingerprintExchangePage);
	}
}

QWizardPage * OtrPeerIdentityVerificationWindow::createQuestionAndAnswerPage()
{
	QWizardPage *page = new QWizardPage;
	page->setTitle(tr("Question And Answer"));

	return page;
}

QWizardPage * OtrPeerIdentityVerificationWindow::createSharedSecretPage()
{
	QWizardPage *page = new QWizardPage;
	page->setTitle(tr("Shared Secret"));

	return page;
}
