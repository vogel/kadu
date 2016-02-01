/*
 * %kadu copyright begin%
 * Copyright 2015 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "injeqt-type-roles.h"

#include <QtCore/QPointer>
#include <injeqt/injeqt.h>

class AccountConfigurationWidgetFactoryRepository;
class BuddyConfigurationWidgetFactoryRepository;
class ChatTopBarWidgetFactoryRepository;
class NotificationEventRepository;
class OtrAccountConfigurationWidgetFactory;
class OtrBuddyConfigurationWidgetFactory;
class OtrChatTopBarWidgetFactory;
class OtrFingerprintService;
class OtrInstanceTagService;
class OtrIsLoggedInService;
class OtrMessageService;
class OtrNotifier;
class OtrPeerIdentityVerificationService;
class OtrPeerIdentityVerificationWindowRepository;
class OtrPrivateKeyService;
class OtrRawMessageTransformer;
class OtrSessionService;
class OtrTrustLevelService;
class RawMessageTransformerService;

class OtrPluginObject : public QObject
{
	Q_OBJECT
	INJEQT_TYPE_ROLE(PLUGIN)

public:
	Q_INVOKABLE explicit OtrPluginObject(QObject *parent = nullptr);
	virtual ~OtrPluginObject();

private:
	QPointer<AccountConfigurationWidgetFactoryRepository> m_accountConfigurationWidgetFactoryRepository;
	QPointer<BuddyConfigurationWidgetFactoryRepository> m_buddyConfigurationWidgetFactoryRepository;
	QPointer<ChatTopBarWidgetFactoryRepository> m_chatTopBarWidgetFactoryRepository;
	QPointer<NotificationEventRepository> m_notificationEventRepository;
	QPointer<OtrAccountConfigurationWidgetFactory> m_otrAccountConfigurationWidgetFactory;
	QPointer<OtrBuddyConfigurationWidgetFactory> m_otrBuddyConfigurationWidgetFactory;
	QPointer<OtrChatTopBarWidgetFactory> m_otrChatTopBarWidgetFactory;
	QPointer<OtrFingerprintService> m_otrFingerprintService;
	QPointer<OtrInstanceTagService> m_otrInstanceTagService;
	QPointer<OtrIsLoggedInService> m_otrIsLoggedInService;
	QPointer<OtrMessageService> m_otrMessageService;
	QPointer<OtrNotifier> m_otrNotifier;
	QPointer<OtrPeerIdentityVerificationService> m_otrPeerIdentityVerificationService;
	QPointer<OtrPeerIdentityVerificationWindowRepository> m_otrPeerIdentityVerificationWindowRepository;
	QPointer<OtrPrivateKeyService> m_otrPrivateKeyService;
	QPointer<OtrRawMessageTransformer> m_otrRawMessageTransformer;
	QPointer<OtrSessionService> m_otrSessionService;
	QPointer<OtrTrustLevelService> m_otrTrustLevelService;
	QPointer<RawMessageTransformerService> m_rawMessageTransformerService;

	bool m_otrAvailable;

	bool fragmentsFixAvailable() const;

private slots:
	INJEQT_INIT void init();
	INJEQT_DONE void done();
	INJEQT_SET void setAccountConfigurationWidgetFactoryRepository(AccountConfigurationWidgetFactoryRepository *accountConfigurationWidgetFactoryRepository);
	INJEQT_SET void setBuddyConfigurationWidgetFactoryRepository(BuddyConfigurationWidgetFactoryRepository *buddyConfigurationWidgetFactoryRepository);
	INJEQT_SET void setChatTopBarWidgetFactoryRepository(ChatTopBarWidgetFactoryRepository *chatTopBarWidgetFactoryRepository);
	INJEQT_SET void setNotificationEventRepository(NotificationEventRepository *notificationEventRepository);
	INJEQT_SET void setOtrAccountConfigurationWidgetFactory(OtrAccountConfigurationWidgetFactory *otrAccountConfigurationWidgetFactory);
	INJEQT_SET void setOtrBuddyConfigurationWidgetFactory(OtrBuddyConfigurationWidgetFactory *otrBuddyConfigurationWidgetFactory);
	INJEQT_SET void setOtrChatTopBarWidgetFactory(OtrChatTopBarWidgetFactory *otrChatTopBarWidgetFactory);
	INJEQT_SET void setOtrFingerprintService(OtrFingerprintService *otrFingerprintService);
	INJEQT_SET void setOtrInstanceTagService(OtrInstanceTagService *otrInstanceTagService);
	INJEQT_SET void setOtrIsLoggedInService(OtrIsLoggedInService *otrIsLoggedInService);
	INJEQT_SET void setOtrMessageService(OtrMessageService *otrMessageService);
	INJEQT_SET void setOtrNotifier(OtrNotifier *otrNotifier);
	INJEQT_SET void setOtrPeerIdentityVerificationService(OtrPeerIdentityVerificationService *otrPeerIdentityVerificationService);
	INJEQT_SET void setOtrPeerIdentityVerificationWindowRepository(OtrPeerIdentityVerificationWindowRepository *otrPeerIdentityVerificationWindowRepository);
	INJEQT_SET void setOtrPrivateKeyService(OtrPrivateKeyService *otrPrivateKeyService);
	INJEQT_SET void setOtrRawMessageTransformer(OtrRawMessageTransformer *otrRawMessageTransformer);
	INJEQT_SET void setOtrSessionService(OtrSessionService *otrSessionService);
	INJEQT_SET void setOtrTrustLevelService(OtrTrustLevelService *otrTrustLevelService);
	INJEQT_SET void setRawMessageTransformerService(RawMessageTransformerService *rawMessageTransformerService);

};
