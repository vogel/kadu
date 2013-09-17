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

#include "gui/windows/otr-peer-identity-verification-window.h"
#include "otr-fingerprint-extractor.h"
#include "otr-fingerprint-trust.h"

#include "otr-peer-identity-verification-window-factory.h"

OtrPeerIdentityVerificationWindowFactory::OtrPeerIdentityVerificationWindowFactory(QObject *parent) :
		QObject(parent)
{
}

OtrPeerIdentityVerificationWindowFactory::~OtrPeerIdentityVerificationWindowFactory()
{
}

void OtrPeerIdentityVerificationWindowFactory::setFingerprintExtractor(OtrFingerprintExtractor *fingerprintExtractor)
{
	FingerprintExtractor = fingerprintExtractor;
}

void OtrPeerIdentityVerificationWindowFactory::setFingerprintTrust(OtrFingerprintTrust *fingerprintTrust)
{
	FingerprintTrust = fingerprintTrust;
}

OtrPeerIdentityVerificationWindow * OtrPeerIdentityVerificationWindowFactory::createWindow(const Contact &contact, QWidget *parent)
{
	if (Windows.contains(contact))
		return Windows.value(contact);

	OtrPeerIdentityVerificationWindow *result = new OtrPeerIdentityVerificationWindow(contact, FingerprintExtractor.data(),
																					  FingerprintTrust.data(), parent);
	connect(result, SIGNAL(destroyed(Contact)), this, SLOT(windowDestroyed(Contact)));
	Windows.insert(contact, result);

	return result;
}

void OtrPeerIdentityVerificationWindowFactory::windowDestroyed(const Contact &contact)
{
	printf("windowDestroyed: %s\n", qPrintable(contact.display(true)));
	Windows.remove(contact);
}
