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
#include <QtWidgets/QProgressBar>
#include <QtWidgets/QVBoxLayout>

#include "gui/windows/otr-peer-identity-verification-window.h"
#include "otr-peer-identity-verification-service.h"
#include "otr-peer-identity-verification-state.h"

#include "otr-peer-identity-verification-progress-page.h"

OtrPeerIdentityVerificationProgressPage::OtrPeerIdentityVerificationProgressPage(const Contact &contact, QWidget *parent) :
		QWizardPage(parent), MyContact(contact)
{
	createGui();
}

OtrPeerIdentityVerificationProgressPage::~OtrPeerIdentityVerificationProgressPage()
{
}

void OtrPeerIdentityVerificationProgressPage::createGui()
{
	setTitle(tr("Verification Progress"));

	QVBoxLayout *layout = new QVBoxLayout(this);

	QLabel *stateLabel = new QLabel(tr("Verification in progres..."));
	stateLabel->setWordWrap(true);

	QProgressBar *stateProgress = new QProgressBar();
	stateProgress->setMaximum(100);

	registerField("progress", stateProgress, "value");

	layout->addWidget(stateLabel);
	layout->addWidget(stateProgress);
}

void OtrPeerIdentityVerificationProgressPage::setState(const OtrPeerIdentityVerificationState &state)
{
	State = state;
	setField("progress", State.percentCompleted());

	if (State.isFinished())
	{
		setField("result", static_cast<int>(State.state()));
		emit finished();
	}
}

void OtrPeerIdentityVerificationProgressPage::setPeerIdentityVerificationService(OtrPeerIdentityVerificationService *peerIdentityVerificationService)
{
	PeerIdentityVerificationService = peerIdentityVerificationService;
}

void OtrPeerIdentityVerificationProgressPage::cancelVerification()
{
	PeerIdentityVerificationService.data()->cancelVerification(MyContact);
}

bool OtrPeerIdentityVerificationProgressPage::canCancelVerification() const
{
	return State.isFinished();
}

void OtrPeerIdentityVerificationProgressPage::rejected()
{
	if (wizard()->currentPage() == this && canCancelVerification())
		cancelVerification();
}

void OtrPeerIdentityVerificationProgressPage::updateContactState(const Contact &contact, const OtrPeerIdentityVerificationState &state)
{
	if (contact == MyContact)
		setState(state);
}

int OtrPeerIdentityVerificationProgressPage::nextId() const
{
	return OtrPeerIdentityVerificationWindow::ResultPage;
}

void OtrPeerIdentityVerificationProgressPage::initializePage()
{
	setState(OtrPeerIdentityVerificationState());
}

bool OtrPeerIdentityVerificationProgressPage::validatePage()
{
	if (!PeerIdentityVerificationService)
		return true;

	return State.isFinished();
}
