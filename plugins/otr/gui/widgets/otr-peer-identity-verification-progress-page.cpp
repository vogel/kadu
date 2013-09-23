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
#include <QtGui/QProgressBar>
#include <QtGui/QVBoxLayout>

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

	StateLabel = new QLabel();
	StateProgress = new QProgressBar();

	layout->addWidget(StateLabel);
	layout->addWidget(StateProgress);
}

int OtrPeerIdentityVerificationProgressPage::nextId() const
{
	return -1;
}

void OtrPeerIdentityVerificationProgressPage::initializePage()
{
}

bool OtrPeerIdentityVerificationProgressPage::validatePage()
{
	return true;
}
