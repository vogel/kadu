/*
 * %kadu copyright begin%
 * Copyright 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#pragma once

#include "contacts/contact.h"

#include <QtCore/QObject>
#include <QtCore/QPointer>
#include <injeqt/injeqt.h>

class OtrFingerprintService;
class OtrPeerIdentityVerificationService;
class OtrPeerIdentityVerificationWindow;
class OtrTrustLevelService;

class OtrPeerIdentityVerificationWindowFactory : public QObject
{
	Q_OBJECT

public:
	Q_INVOKABLE OtrPeerIdentityVerificationWindowFactory();
	virtual ~OtrPeerIdentityVerificationWindowFactory();

	OtrPeerIdentityVerificationWindow * windowForContact(const Contact &contact);

private slots:
	INJEQT_SET void setFingerprintService(OtrFingerprintService *fingerprintService);
	INJEQT_SET void setPeerIdentityVerificationService(OtrPeerIdentityVerificationService *peerIdentityVerificationService);
	INJEQT_SET void setTrustLevelService(OtrTrustLevelService *trustLevelService);

private:
	QPointer<OtrFingerprintService> FingerprintService;
	QPointer<OtrPeerIdentityVerificationService> PeerIdentityVerificationService;
	QPointer<OtrTrustLevelService> TrustLevelService;

	QMap<Contact, OtrPeerIdentityVerificationWindow *> Windows;

};
