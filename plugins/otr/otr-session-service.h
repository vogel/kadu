/*
 * %kadu copyright begin%
 * Copyright 2004 Adrian Smarzewski (adrian@kadu.net)
 * Copyright 2007, 2008, 2009, 2010, 2011, 2012, 2013 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#ifndef OTR_SESSION_SERVICE_H
#define OTR_SESSION_SERVICE_H

#include <QtCore/QObject>
#include <QtCore/QPointer>

extern "C" {
#	include <libotr/proto.h>
}

class Contact;
class MessageManager;

class OtrAppOpsService;
class OtrOpDataFactory;
class OtrPolicyService;
class OtrTrustLevelService;
class OtrUserStateService;

class OtrSessionService : public QObject
{
	Q_OBJECT

	QPointer<MessageManager> CurrentMessageManager;
	QPointer<OtrAppOpsService> AppOpsService;
	QPointer<OtrOpDataFactory> OpDataFactory;
	QPointer<OtrPolicyService> PolicyService;
	QPointer<OtrTrustLevelService> TrustLevelService;
	QPointer<OtrUserStateService> UserStateService;

public:
	static void wrapperOtrGoneSecure(void *data, ConnContext *context);
	static void wrapperOtrGoneInsecure(void *data, ConnContext *context);
	static void wrapperOtrStillSecure(void *data, ConnContext *context, int isReply);

	explicit OtrSessionService(QObject *parent = 0);
	virtual ~OtrSessionService();

	void setAppOpsService(OtrAppOpsService *appOpsService);
	void setMessageManager(MessageManager *messageManager);
	void setOpDataFactory(OtrOpDataFactory *opDataFactory);
	void setPolicyService(OtrPolicyService *policyService);
	void setTrustLevelService(OtrTrustLevelService *trustLevelService);
	void setUserStateService(OtrUserStateService *userStateService);

public slots:
	void startSession(const Contact &contact);
	void endSession(const Contact &contact);

signals:
	void tryingToStartSession(const Contact &contact) const;
	void tryingToRefreshSession(const Contact &contact) const;

	void goneSecure(const Contact &contact) const;
	void goneInsecure(const Contact &contact) const;
	void stillSecure(const Contact &contact) const;

};

#endif // OTR_SESSION_SERVICE_H
