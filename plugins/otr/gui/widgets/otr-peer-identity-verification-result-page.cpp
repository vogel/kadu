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
#include <QtGui/QVBoxLayout>

#include "gui/windows/otr-peer-identity-verification-window.h"
#include "otr-peer-identity-verification-state.h"

#include "otr-peer-identity-verification-result-page.h"

OtrPeerIdentityVerificationResultPage::OtrPeerIdentityVerificationResultPage(QWidget *parent) :
		QWizardPage(parent)
{
	setTitle(tr("Verification Result"));

	createGui();
}

OtrPeerIdentityVerificationResultPage::~OtrPeerIdentityVerificationResultPage()
{
}

void OtrPeerIdentityVerificationResultPage::createGui()
{
	QLabel *resultLabel = new QLabel();
	resultLabel->setWordWrap(true);

	registerField("result", resultLabel);
	registerField("resultText", resultLabel, "text");

	QVBoxLayout *layout = new QVBoxLayout(this);
	layout->addWidget(resultLabel);
}

int OtrPeerIdentityVerificationResultPage::nextId() const
{
	return -1;
}

void OtrPeerIdentityVerificationResultPage::initializePage()
{
	setField("resultText", stateToString(static_cast<OtrPeerIdentityVerificationState::State>(field("result").toInt())));
}

QString OtrPeerIdentityVerificationResultPage::stateToString(const OtrPeerIdentityVerificationState::State &state)
{
	switch (state)
	{
		case OtrPeerIdentityVerificationState::StateFailed:
			return tr("Verification failed. You are probably talking to an imposter. Either close conversation or try other verification method.");
		case OtrPeerIdentityVerificationState::StateSucceeded:
			return tr("Verification succeeded.");
		default:
			return tr("Unknown.");
	}
}
