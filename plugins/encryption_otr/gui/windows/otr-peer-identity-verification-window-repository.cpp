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

#include "gui/windows/otr-peer-identity-verification-window-factory.h"
#include "gui/windows/otr-peer-identity-verification-window.h"

#include "otr-peer-identity-verification-window-repository.h"

OtrPeerIdentityVerificationWindowRepository::OtrPeerIdentityVerificationWindowRepository(QObject *parent) :
		QObject(parent)
{
}

OtrPeerIdentityVerificationWindowRepository::~OtrPeerIdentityVerificationWindowRepository()
{
}

void OtrPeerIdentityVerificationWindowRepository::setPeerIdentityVerificationWindowFactory(OtrPeerIdentityVerificationWindowFactory *peerIdentityVerificationWindowFactory)
{
	PeerIdentityVerificationWindowFactory = peerIdentityVerificationWindowFactory;
}

OtrPeerIdentityVerificationWindow * OtrPeerIdentityVerificationWindowRepository::windowForContact(const Contact &contact)
{
	if (Windows.contains(contact))
		return Windows.value(contact);

	if (!PeerIdentityVerificationWindowFactory)
		return 0;


	OtrPeerIdentityVerificationWindow *result = PeerIdentityVerificationWindowFactory.data()->windowForContact(contact);
	connect(result, SIGNAL(destroyed(Contact)), this, SLOT(windowDestroyed(Contact)));
	Windows.insert(contact, result);

	return result;
}

void OtrPeerIdentityVerificationWindowRepository::windowDestroyed(const Contact &contact)
{
	Windows.remove(contact);
}

void OtrPeerIdentityVerificationWindowRepository::showRespondQuestionAndAnswerVerificationWindow(const Contact &contact, const QString &question)
{
	OtrPeerIdentityVerificationWindow *window = windowForContact(contact);
	if (window)
	{
		window->showRespondQuestionAndAnswer(question);
		window->raise();
	}
}

void OtrPeerIdentityVerificationWindowRepository::showRespondSharedSecretVerificationWindow(const Contact &contact)
{
	OtrPeerIdentityVerificationWindow *window = windowForContact(contact);
	if (window)
	{
		window->showRespondSharedSecret();
		window->raise();
	}
}

void OtrPeerIdentityVerificationWindowRepository::showVerificationWindow(const Contact &contact)
{
	OtrPeerIdentityVerificationWindow *window = windowForContact(contact);
	if (window)
	{
		window->show();
		window->raise();
	}
}
