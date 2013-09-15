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

#include <QtGui/QRadioButton>
#include <QtGui/QVBoxLayout>

#include "gui/windows/otr-peer-identity-verification-window.h"

#include "otr-peer-identity-verification-select-method-page.h"

OtrPeerIdentityVerificationSelectMethodPage::OtrPeerIdentityVerificationSelectMethodPage(QWidget *parent) :
		QWizardPage(parent)
{
	setTitle(tr("Select Verification Method"));

	createGui();
}

OtrPeerIdentityVerificationSelectMethodPage::~OtrPeerIdentityVerificationSelectMethodPage()
{
}

void OtrPeerIdentityVerificationSelectMethodPage::createGui()
{
	QRadioButton *questionAndAnswer = new QRadioButton(tr("Question and Answer"));
	QRadioButton *sharedSecread = new QRadioButton(tr("Shared Secred"));
	QRadioButton *fingerprintExchange = new QRadioButton(tr("Fingerprint Exchange"));

	registerField("questionAndAnswer", questionAndAnswer);
	registerField("sharedSecread", sharedSecread);
	registerField("fingerprintExchange", fingerprintExchange);

	QVBoxLayout *layout = new QVBoxLayout(this);
	layout->addWidget(questionAndAnswer);
	layout->addWidget(sharedSecread);
	layout->addWidget(fingerprintExchange);

	questionAndAnswer->setChecked(true);
}

int OtrPeerIdentityVerificationSelectMethodPage::nextId() const
{
	if (field("questionAndAnswer").toBool())
		return OtrPeerIdentityVerificationWindow::QuestionAndAnswerPage;
	if (field("sharedSecread").toBool())
		return OtrPeerIdentityVerificationWindow::SharedSecretPage;
	if (field("fingerprintExchange").toBool())
		return OtrPeerIdentityVerificationWindow::FingerprintExchangePage;

	return -1;
}
