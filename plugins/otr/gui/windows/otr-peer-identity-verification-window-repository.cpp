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
#include "gui/widgets/otr-peer-identity-verification-question-and-answer-page.h"
#include "gui/windows/otr-peer-identity-verification-window.h"
#include "otr-app-ops-wrapper.h"
#include "otr-fingerprint-service.h"

#include "otr-peer-identity-verification-window-repository.h"

OtrPeerIdentityVerificationWindowRepository::OtrPeerIdentityVerificationWindowRepository(QObject *parent) :
		QObject(parent)
{
}

OtrPeerIdentityVerificationWindowRepository::~OtrPeerIdentityVerificationWindowRepository()
{
}

void OtrPeerIdentityVerificationWindowRepository::setAppOpsWrapper(OtrAppOpsWrapper *appOpsWrapper)
{
	AppOpsWrapper = appOpsWrapper;
}

void OtrPeerIdentityVerificationWindowRepository::setFingerprintService(OtrFingerprintService *fingerprintService)
{
	FingerprintService = fingerprintService;
}

OtrPeerIdentityVerificationWindow * OtrPeerIdentityVerificationWindowRepository::windowForContact(const Contact &contact)
{
	if (Windows.contains(contact))
		return Windows.value(contact);

	OtrPeerIdentityVerificationWindow *result = new OtrPeerIdentityVerificationWindow(contact);

	OtrPeerIdentityVerificationQuestionAndAnswerPage *questionAndAnswerPage = new OtrPeerIdentityVerificationQuestionAndAnswerPage(contact, result);
	questionAndAnswerPage->setAppOpsWrapper(AppOpsWrapper.data());
	result->setPage(OtrPeerIdentityVerificationWindow::QuestionAndAnswerPage, questionAndAnswerPage);

	OtrPeerIdentityVerificationFingerprintExchangePage *fingerprintExchangePage = new OtrPeerIdentityVerificationFingerprintExchangePage(contact, result);
	fingerprintExchangePage->setFingerprintService(FingerprintService.data());
	result->setPage(OtrPeerIdentityVerificationWindow::FingerprintExchangePage, fingerprintExchangePage);

	connect(result, SIGNAL(destroyed(Contact)), this, SLOT(windowDestroyed(Contact)));
	Windows.insert(contact, result);

	return result;
}

void OtrPeerIdentityVerificationWindowRepository::windowDestroyed(const Contact &contact)
{
	Windows.remove(contact);
}

void OtrPeerIdentityVerificationWindowRepository::showVerificationWindow(const Contact &contact)
{
	OtrPeerIdentityVerificationWindow *window = windowForContact(contact);
	if (window)
		window->show();
}
