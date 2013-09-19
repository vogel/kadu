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

#include <QtGui/QFormLayout>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QRadioButton>
#include <QtGui/QPushButton>

#include "accounts/account.h"
#include "protocols/protocol.h"
#include "protocols/protocol-factory.h"

#include "otr-app-ops-wrapper.h"
#include "otr-fingerprint-service.h"

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
	setTitle(tr("Question and Answer"));

	QFormLayout *layout = new QFormLayout(this);

	QuestionEdit = new QLineEdit();
	AnswerEdit = new QLineEdit();
	QPushButton *askQuestionButton = new QPushButton(tr("Ask Question"));
	connect(askQuestionButton, SIGNAL(clicked(bool)), this, SLOT(askQuestion()));

	layout->addRow(new QLabel(tr("Question:")), QuestionEdit);
	layout->addRow(new QLabel(tr("Answer:")), AnswerEdit);
	layout->addRow(0, askQuestionButton);

	registerField("question", QuestionEdit);
	registerField("answer", AnswerEdit);
}

void OtrPeerIdentityVerificationQuestionAndAnswerPage::setAppOpsWrapper(OtrAppOpsWrapper *appOpsWrapper)
{
	AppOpsWrapper = appOpsWrapper;
}

void OtrPeerIdentityVerificationQuestionAndAnswerPage::askQuestion()
{
	if (AppOpsWrapper)
		AppOpsWrapper.data()->startSMPAskQuestion(MyContact, field("question").toString(), field("answer").toString());
}

int OtrPeerIdentityVerificationQuestionAndAnswerPage::nextId() const
{
	return -1;
}

void OtrPeerIdentityVerificationQuestionAndAnswerPage::initializePage()
{
}

bool OtrPeerIdentityVerificationQuestionAndAnswerPage::validatePage()
{
	return false;
}
