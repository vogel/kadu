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

#include <QtWidgets/QGridLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>

#include "accounts/account.h"
#include "protocols/protocol-factory.h"
#include "protocols/protocol.h"

#include "gui/windows/otr-peer-identity-verification-window.h"
#include "otr-fingerprint-service.h"
#include "otr-peer-identity-verification-service.h"

#include "otr-peer-identity-verification-respond-question-and-answer-page.h"

OtrPeerIdentityVerificationRespondQuestionAndAnswerPage::OtrPeerIdentityVerificationRespondQuestionAndAnswerPage(const Contact &contact, QWidget *parent) :
		QWizardPage(parent), MyContact(contact)
{
	createGui();
}

OtrPeerIdentityVerificationRespondQuestionAndAnswerPage::~OtrPeerIdentityVerificationRespondQuestionAndAnswerPage()
{
}

void OtrPeerIdentityVerificationRespondQuestionAndAnswerPage::createGui()
{
	setButtonText(QWizard::CommitButton, tr("Answer Question"));
	setCommitPage(true);
	setTitle(tr("Respond to Question and Answer"));

	QGridLayout *layout = new QGridLayout(this);

	QuestionLabel = new QLabel();
	QuestionLabel->setWordWrap(true);

	QLineEdit *answerEdit = new QLineEdit();

	layout->setColumnStretch(0, 1);
	layout->setColumnStretch(1, 100);

	layout->addWidget(new QLabel(tr("%1 wants to verify your identity using Question and Answer method.").arg(MyContact.display(true))), 0, 0, 1, 2);
	layout->addWidget(new QLabel(tr("%1 question:").arg(MyContact.display(true))), 1, 0, 1, 1, Qt::AlignTop);
	layout->addWidget(QuestionLabel, 1, 1, 1, 1);
	layout->addWidget(new QLabel(tr("Answer:")), 2, 0, 1, 2);
	layout->addWidget(answerEdit, 3, 0, 1, 2);

	registerField("respondQuestion", QuestionLabel, "text");
	registerField("respondAnswer*", answerEdit);
}

void OtrPeerIdentityVerificationRespondQuestionAndAnswerPage::setPeerIdentityVerificationService(OtrPeerIdentityVerificationService *peerIdentityVerificationService)
{
	PeerIdentityVerificationService = peerIdentityVerificationService;
}

void OtrPeerIdentityVerificationRespondQuestionAndAnswerPage::rejected()
{
	if (!PeerIdentityVerificationService)
		return;

	if (wizard()->currentPage() == this)
		PeerIdentityVerificationService.data()->cancelVerification(MyContact);
}

int OtrPeerIdentityVerificationRespondQuestionAndAnswerPage::nextId() const
{
	return OtrPeerIdentityVerificationWindow::ProgressPage;
}

void OtrPeerIdentityVerificationRespondQuestionAndAnswerPage::initializePage()
{
	setField("respondAnswer", QString());
}

bool OtrPeerIdentityVerificationRespondQuestionAndAnswerPage::validatePage()
{
	QString respondAnswer = field("respondAnswer").toString();

	if (respondAnswer.isEmpty())
		return false;

	if (PeerIdentityVerificationService)
		PeerIdentityVerificationService.data()->respondVerification(MyContact, respondAnswer);

	return true;
}
