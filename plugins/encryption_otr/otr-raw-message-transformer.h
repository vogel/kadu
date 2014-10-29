/*
 * %kadu copyright begin%
 * Copyright 2004 Adrian Smarzewski (adrian@kadu.net)
 * Copyright 2007, 2008, 2009, 2010, 2011, 2012, 2013, 2014 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2014 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#pragma once

#include <QtCore/QPointer>
#include <injeqt/injeqt.h>

#include "protocols/services/raw-message-transformer.h"

extern "C" {
#	include <libotr/proto.h>
#	include <libotr/message.h>
}

class Contact;

class OtrAppOpsService;
class OtrOpDataFactory;
class OtrSessionService;
class OtrUserStateService;

class OtrRawMessageTransformer: public QObject, public RawMessageTransformer
{
	Q_OBJECT

public:
	Q_INVOKABLE OtrRawMessageTransformer();
	virtual ~OtrRawMessageTransformer();

	void setEnableFragments(bool enableFragments);

	virtual RawMessage transform(const RawMessage &rawMessage, const Message &message) override;

signals:
	void peerEndedSession(const Contact &contact) const;

private slots:
	INJEQT_SETTER void setAppOpsService(OtrAppOpsService *appOpsService);
	INJEQT_SETTER void setOpDataFactory(OtrOpDataFactory *opDataFactory);
	INJEQT_SETTER void setSessionService(OtrSessionService *sessionService);
	INJEQT_SETTER void setUserStateService(OtrUserStateService *userStateService);

private:
	QPointer<OtrAppOpsService> AppOpsService;
	QPointer<OtrOpDataFactory> OpDataFactory;
	QPointer<OtrSessionService> SessionService;
	QPointer<OtrUserStateService> UserStateService;

	bool EnableFragments;

	RawMessage transformReceived(const RawMessage &RawMessage, const Message &message);
	RawMessage transformSent(const RawMessage &rawMessage, const Message &message);

};
