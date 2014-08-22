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

#include <QtGui/QGridLayout>
#include <QtGui/QLabel>
#include <QtGui/QRadioButton>
#include <QtGui/QStyle>

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

	QGridLayout *layout = new QGridLayout(this);
	layout->setColumnMinimumWidth(0, style()->pixelMetric(QStyle::PM_ExclusiveIndicatorWidth));
	layout->addWidget(questionAndAnswer, 0, 0, 1, 2);
	layout->addWidget(createDescirptionLabel(tr("Ask peer a question that only you and he/she know the answer for. If your peer answer it properly, he/she will be marked as verified.")), 1, 1, 1, 1);
	layout->setRowMinimumHeight(2, 8);
	layout->addWidget(sharedSecread, 3, 0, 1, 2);
	layout->addWidget(createDescirptionLabel(tr("Exchange a secret message with peer. This message should be agreed to on another secure channel (face-to-face conversation or a phone).")), 4, 1, 1, 1);
	layout->setRowMinimumHeight(5, 8);
	layout->addWidget(fingerprintExchange, 6, 0, 1, 2);
	layout->addWidget(createDescirptionLabel(tr("Check your peer's fingerprint manually. Fingreprint information shoud be exchanged on another secure channel (face-to-face conversation or a phone).")), 7, 1, 1, 1);

	questionAndAnswer->setChecked(true);
}

QLabel * OtrPeerIdentityVerificationSelectMethodPage::createDescirptionLabel(const QString &text)
{
	QLabel *result = new QLabel(text);
	result->setWordWrap(true);
	return result;
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
