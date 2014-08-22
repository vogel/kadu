/*
 * %kadu copyright begin%
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2004 Adrian Smarzewski (adrian@kadu.net)
 * Copyright 2007, 2008, 2009, 2010, 2011, 2012, 2013, 2014 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2004, 2006 Marcin Ślusarz (joi@kadu.net)
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
#include <QtWidgets/QVBoxLayout>

#include "gui/windows/otr-peer-identity-verification-window.h"
#include "otr-peer-identity-verification-state.h"

#include "otr-peer-identity-verification-result-page.h"

OtrPeerIdentityVerificationResultPage::OtrPeerIdentityVerificationResultPage(const Contact &contact, QWidget *parent) :
		QWizardPage(parent), MyContact(contact)
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

void OtrPeerIdentityVerificationResultPage::setTrustLevelService(OtrTrustLevelService *trustLevelService)
{
	TrustLevelService = trustLevelService;
}

int OtrPeerIdentityVerificationResultPage::nextId() const
{
	return -1;
}

void OtrPeerIdentityVerificationResultPage::initializePage()
{
	OtrPeerIdentityVerificationState::State result = static_cast<OtrPeerIdentityVerificationState::State>(field("result").toInt());
	OtrTrustLevelService::TrustLevel trustLevel = TrustLevelService
			? TrustLevelService.data()->loadTrustLevelFromContact(MyContact)
			: OtrTrustLevelService::TrustLevelUnknown;
	
	setField("resultText", stateToString(result, trustLevel));
}

QString OtrPeerIdentityVerificationResultPage::stateToString(const OtrPeerIdentityVerificationState::State &state,
															 OtrTrustLevelService::TrustLevel trustLevel)
{
	if (OtrPeerIdentityVerificationState::StateFailed == state)
		return tr("Verificationof %1 failed. You are probably talking to an imposter. Either close conversation or try other verification method.").arg(MyContact.display(true));
	if (OtrPeerIdentityVerificationState::StateSucceeded == state && OtrTrustLevelService::TrustLevelPrivate != trustLevel)
		return tr("%1 has verified you. You should do the same.").arg(MyContact.display(true));
	if (OtrPeerIdentityVerificationState::StateSucceeded == state)
		return tr("Verification of %1 succeeded.").arg(MyContact.display(true));
	return tr("Unknown.");
}
