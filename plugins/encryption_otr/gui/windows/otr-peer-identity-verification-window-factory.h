/*
 * %kadu copyright begin%
 * Copyright 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2009, 2010 Tomasz Rostański (rozteck@interia.pl)
 * Copyright 2011, 2013, 2014 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2010, 2014 Bartosz Brachaczek (b.brachaczek@gmail.com)
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
#include <QtCore/QPointer>

#include "contacts/contact.h"

class OtrFingerprintService;
class OtrPeerIdentityVerificationService;
class OtrPeerIdentityVerificationWindow;
class OtrTrustLevelService;

class OtrPeerIdentityVerificationWindowFactory : public QObject
{
	Q_OBJECT

	QPointer<OtrFingerprintService> FingerprintService;
	QPointer<OtrPeerIdentityVerificationService> PeerIdentityVerificationService;
	QPointer<OtrTrustLevelService> TrustLevelService;

	QMap<Contact, OtrPeerIdentityVerificationWindow *> Windows;

public:
	explicit OtrPeerIdentityVerificationWindowFactory(QObject *parent = 0);
	virtual ~OtrPeerIdentityVerificationWindowFactory();

	void setFingerprintService(OtrFingerprintService *fingerprintService);
	void setPeerIdentityVerificationService(OtrPeerIdentityVerificationService *peerIdentityVerificationService);
	void setTrustLevelService(OtrTrustLevelService *trustLevelService);

	OtrPeerIdentityVerificationWindow * windowForContact(const Contact &contact);

};

#endif // OTR_PEER_IDENTITY_VERIFICATION_WINDOW_FACTORY_H
