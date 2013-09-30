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
#include "gui/widgets/otr-peer-identity-verification-progress-page.h"
#include "gui/widgets/otr-peer-identity-verification-question-and-answer-page.h"
#include "gui/widgets/otr-peer-identity-verification-respond-question-and-answer-page.h"
#include "gui/widgets/otr-peer-identity-verification-respond-shared-secret-page.h"
#include "gui/widgets/otr-peer-identity-verification-result-page.h"
#include "gui/widgets/otr-peer-identity-verification-select-method-page.h"
#include "gui/widgets/otr-peer-identity-verification-shared-secret-page.h"
#include "gui/windows/otr-peer-identity-verification-window.h"
#include "otr-fingerprint-service.h"
#include "otr-peer-identity-verification-service.h"

#include "otr-peer-identity-verification-window-factory.h"

OtrPeerIdentityVerificationWindowFactory::OtrPeerIdentityVerificationWindowFactory(QObject *parent) :
		QObject(parent)
{
}

OtrPeerIdentityVerificationWindowFactory::~OtrPeerIdentityVerificationWindowFactory()
{
}

void OtrPeerIdentityVerificationWindowFactory::setFingerprintService(OtrFingerprintService *fingerprintService)
{
	FingerprintService = fingerprintService;
}

void OtrPeerIdentityVerificationWindowFactory::setPeerIdentityVerificationService(OtrPeerIdentityVerificationService *peerIdentityVerificationService)
{
	PeerIdentityVerificationService = peerIdentityVerificationService;
}

void OtrPeerIdentityVerificationWindowFactory::setTrustLevelService(OtrTrustLevelService *trustLevelService)
{
	TrustLevelService = trustLevelService;
}

OtrPeerIdentityVerificationWindow * OtrPeerIdentityVerificationWindowFactory::windowForContact(const Contact &contact)
{
	OtrPeerIdentityVerificationWindow *result = new OtrPeerIdentityVerificationWindow(contact);

	result->setPage(OtrPeerIdentityVerificationWindow::SelectMethodPage, new OtrPeerIdentityVerificationSelectMethodPage(result));

	OtrPeerIdentityVerificationFingerprintExchangePage *fingerprintExchangePage = new OtrPeerIdentityVerificationFingerprintExchangePage(contact, result);
	fingerprintExchangePage->setFingerprintService(FingerprintService.data());
	result->setPage(OtrPeerIdentityVerificationWindow::FingerprintExchangePage, fingerprintExchangePage);

	OtrPeerIdentityVerificationQuestionAndAnswerPage *questionAndAnswerPage = new OtrPeerIdentityVerificationQuestionAndAnswerPage(contact, result);
	questionAndAnswerPage->setPeerIdentityVerificationService(PeerIdentityVerificationService.data());
	result->setPage(OtrPeerIdentityVerificationWindow::QuestionAndAnswerPage, questionAndAnswerPage);

	OtrPeerIdentityVerificationSharedSecretPage *sharedSecretPage = new OtrPeerIdentityVerificationSharedSecretPage(contact, result);
	sharedSecretPage->setPeerIdentityVerificationService(PeerIdentityVerificationService.data());
	result->setPage(OtrPeerIdentityVerificationWindow::SharedSecretPage, sharedSecretPage);

	OtrPeerIdentityVerificationRespondQuestionAndAnswerPage *respondQuestionAndAnswerPage = new OtrPeerIdentityVerificationRespondQuestionAndAnswerPage(contact, result);
	respondQuestionAndAnswerPage->setPeerIdentityVerificationService(PeerIdentityVerificationService.data());
	connect(result, SIGNAL(aboutToBeRejected()), respondQuestionAndAnswerPage, SLOT(rejected()));
	result->setPage(OtrPeerIdentityVerificationWindow::RespondQuestionAndAnswerPage, respondQuestionAndAnswerPage);

	OtrPeerIdentityVerificationRespondSharedSecretPage *respondSharedSecretPage = new OtrPeerIdentityVerificationRespondSharedSecretPage(contact, result);
	respondSharedSecretPage->setPeerIdentityVerificationService(PeerIdentityVerificationService.data());
	connect(result, SIGNAL(aboutToBeRejected()), respondQuestionAndAnswerPage, SLOT(rejected()));
	result->setPage(OtrPeerIdentityVerificationWindow::RespondSharedSecretPage, respondSharedSecretPage);

	OtrPeerIdentityVerificationProgressPage *progressPage = new OtrPeerIdentityVerificationProgressPage(contact, result);
	progressPage->setPeerIdentityVerificationService(PeerIdentityVerificationService.data());
	connect(PeerIdentityVerificationService.data(), SIGNAL(contactStateUpdated(Contact,OtrPeerIdentityVerificationState)),
			progressPage, SLOT(updateContactState(Contact,OtrPeerIdentityVerificationState)));
	connect(result, SIGNAL(aboutToBeRejected()), progressPage, SLOT(rejected()));
	connect(progressPage, SIGNAL(finished()), result, SLOT(next()));
	result->setPage(OtrPeerIdentityVerificationWindow::ProgressPage, progressPage);

	OtrPeerIdentityVerificationResultPage *resultPage = new OtrPeerIdentityVerificationResultPage(contact, result);
	resultPage->setTrustLevelService(TrustLevelService.data());
	result->setPage(OtrPeerIdentityVerificationWindow::ResultPage, resultPage);

	return result;
}
