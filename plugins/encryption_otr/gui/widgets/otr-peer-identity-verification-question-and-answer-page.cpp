/*
 * %kadu copyright begin%
 * Copyright 2013, 2014 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "otr-peer-identity-verification-question-and-answer-page.h"

OtrPeerIdentityVerificationQuestionAndAnswerPage::OtrPeerIdentityVerificationQuestionAndAnswerPage(const Contact &contact, QWidget *parent) :
		QWizardPage(parent), MyContact(contact)
{
	createGui();
}

OtrPeerIdentityVerificationQuestionAndAnswerPage::~OtrPeerIdentityVerificationQuestionAndAnswerPage()
{
}

void OtrPeerIdentityVerificationQuestionAndAnswerPage::createGui()
{
	setButtonText(QWizard::CommitButton, tr("Ask Question"));
	setCommitPage(true);
	setTitle(tr("Question and Answer"));

	QVBoxLayout *layout = new QVBoxLayout(this);

	QLineEdit *questionEdit = new QLineEdit();
	QLineEdit *answerEdit = new QLineEdit();

	layout->addWidget(new QLabel(tr("Question that only %1 can answer:").arg(MyContact.display(1))));
	layout->addWidget(questionEdit);
	layout->addWidget(new QLabel(tr("Answer:")));
	layout->addWidget(answerEdit);

	registerField("question*", questionEdit);
	registerField("answer*", answerEdit);
}

void OtrPeerIdentityVerificationQuestionAndAnswerPage::setPeerIdentityVerificationService(OtrPeerIdentityVerificationService *peerIdentityVerificationService)
{
	PeerIdentityVerificationService = peerIdentityVerificationService;
}

int OtrPeerIdentityVerificationQuestionAndAnswerPage::nextId() const
{
	return OtrPeerIdentityVerificationWindow::ProgressPage;
}

void OtrPeerIdentityVerificationQuestionAndAnswerPage::initializePage()
{
	setField("question", QString());
	setField("answer", QString());
}

bool OtrPeerIdentityVerificationQuestionAndAnswerPage::validatePage()
{
	QString question = field("question").toString();
	QString answer = field("answer").toString();

	if (question.isEmpty() || answer.isEmpty())
		return false;

	if (PeerIdentityVerificationService)
		PeerIdentityVerificationService.data()->startQuestionAndAnswerVerification(MyContact, field("question").toString(), field("answer").toString());

	return true;
}
