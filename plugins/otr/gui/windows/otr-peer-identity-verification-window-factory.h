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

#ifndef OTR_PEER_IDENTITY_VERIFICATION_WINDOW_FACTORY_H
#define OTR_PEER_IDENTITY_VERIFICATION_WINDOW_FACTORY_H

#include <QtCore/QObject>

#include "contacts/contact.h"

class OtrFingerprintService;
class OtrPeerIdentityVerificationService;
class OtrPeerIdentityVerificationWindow;

class OtrPeerIdentityVerificationWindowFactory : public QObject
{
	Q_OBJECT

	QWeakPointer<OtrFingerprintService> FingerprintService;
	QWeakPointer<OtrPeerIdentityVerificationService> PeerIdentityVerificationService;
	QMap<Contact, OtrPeerIdentityVerificationWindow *> Windows;

public:
	explicit OtrPeerIdentityVerificationWindowFactory(QObject *parent = 0);
	virtual ~OtrPeerIdentityVerificationWindowFactory();

	void setFingerprintService(OtrFingerprintService *fingerprintService);
	void setPeerIdentityVerificationService(OtrPeerIdentityVerificationService *peerIdentityVerificationService);

	OtrPeerIdentityVerificationWindow * windowForContact(const Contact &contact);

};

#endif // OTR_PEER_IDENTITY_VERIFICATION_WINDOW_FACTORY_H
