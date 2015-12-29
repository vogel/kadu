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

#include "plugin/plugin-object.h"

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

class OtrPluginObject : public PluginObject
{
	Q_OBJECT

public:
	Q_INVOKABLE explicit OtrPluginObject(QObject *parent = nullptr);
	virtual ~OtrPluginObject();

	virtual void init();
	virtual void done();

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
	INJEQT_SETTER void setAccountConfigurationWidgetFactoryRepository(AccountConfigurationWidgetFactoryRepository *accountConfigurationWidgetFactoryRepository);
	INJEQT_SETTER void setBuddyConfigurationWidgetFactoryRepository(BuddyConfigurationWidgetFactoryRepository *buddyConfigurationWidgetFactoryRepository);
	INJEQT_SETTER void setChatTopBarWidgetFactoryRepository(ChatTopBarWidgetFactoryRepository *chatTopBarWidgetFactoryRepository);
	INJEQT_SETTER void setNotificationEventRepository(NotificationEventRepository *notificationEventRepository);
	INJEQT_SETTER void setOtrAccountConfigurationWidgetFactory(OtrAccountConfigurationWidgetFactory *otrAccountConfigurationWidgetFactory);
	INJEQT_SETTER void setOtrBuddyConfigurationWidgetFactory(OtrBuddyConfigurationWidgetFactory *otrBuddyConfigurationWidgetFactory);
	INJEQT_SETTER void setOtrChatTopBarWidgetFactory(OtrChatTopBarWidgetFactory *otrChatTopBarWidgetFactory);
	INJEQT_SETTER void setOtrFingerprintService(OtrFingerprintService *otrFingerprintService);
	INJEQT_SETTER void setOtrInstanceTagService(OtrInstanceTagService *otrInstanceTagService);
	INJEQT_SETTER void setOtrIsLoggedInService(OtrIsLoggedInService *otrIsLoggedInService);
	INJEQT_SETTER void setOtrMessageService(OtrMessageService *otrMessageService);
	INJEQT_SETTER void setOtrNotifier(OtrNotifier *otrNotifier);
	INJEQT_SETTER void setOtrPeerIdentityVerificationService(OtrPeerIdentityVerificationService *otrPeerIdentityVerificationService);
	INJEQT_SETTER void setOtrPeerIdentityVerificationWindowRepository(OtrPeerIdentityVerificationWindowRepository *otrPeerIdentityVerificationWindowRepository);
	INJEQT_SETTER void setOtrPrivateKeyService(OtrPrivateKeyService *otrPrivateKeyService);
	INJEQT_SETTER void setOtrRawMessageTransformer(OtrRawMessageTransformer *otrRawMessageTransformer);
	INJEQT_SETTER void setOtrSessionService(OtrSessionService *otrSessionService);
	INJEQT_SETTER void setOtrTrustLevelService(OtrTrustLevelService *otrTrustLevelService);
	INJEQT_SETTER void setRawMessageTransformerService(RawMessageTransformerService *rawMessageTransformerService);

};
