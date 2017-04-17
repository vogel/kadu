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

#include "otr-plugin-object.h"
#include "otr-plugin-object.moc"

#include "gui/widgets/otr-account-configuration-widget-factory.h"
#include "gui/widgets/otr-buddy-configuration-widget-factory.h"
#include "gui/widgets/otr-chat-top-bar-widget-factory.h"
#include "gui/windows/otr-peer-identity-verification-window-repository.h"
#include "otr-fingerprint-service.h"
#include "otr-instance-tag-service.h"
#include "otr-is-logged-in-service.h"
#include "otr-message-service.h"
#include "otr-notifier.h"
#include "otr-peer-identity-verification-service.h"
#include "otr-private-key-service.h"
#include "otr-raw-message-transformer.h"
#include "otr-session-service.h"
#include "otr-trust-level-service.h"

#include "message/message-manager.h"
#include "notification/notification-event-repository.h"
#include "plugin/activation/plugin-activation-error-exception.h"
#include "services/raw-message-transformer-service.h"
#include "widgets/account-configuration-widget-factory-repository.h"
#include "widgets/buddy-configuration-widget-factory-repository.h"
#include "widgets/chat-top-bar-widget-factory-repository.h"

#include <libotr/proto.h>

OtrPluginObject::OtrPluginObject(QObject *parent) : QObject{parent}
{
    m_otrAvailable = otrl_init(OTRL_VERSION_MAJOR, OTRL_VERSION_MINOR, OTRL_VERSION_SUB) == 0;
}

OtrPluginObject::~OtrPluginObject()
{
}

void OtrPluginObject::setAccountConfigurationWidgetFactoryRepository(
    AccountConfigurationWidgetFactoryRepository *accountConfigurationWidgetFactoryRepository)
{
    m_accountConfigurationWidgetFactoryRepository = accountConfigurationWidgetFactoryRepository;
}

void OtrPluginObject::setBuddyConfigurationWidgetFactoryRepository(
    BuddyConfigurationWidgetFactoryRepository *buddyConfigurationWidgetFactoryRepository)
{
    m_buddyConfigurationWidgetFactoryRepository = buddyConfigurationWidgetFactoryRepository;
}

void OtrPluginObject::setChatTopBarWidgetFactoryRepository(
    ChatTopBarWidgetFactoryRepository *chatTopBarWidgetFactoryRepository)
{
    m_chatTopBarWidgetFactoryRepository = chatTopBarWidgetFactoryRepository;
}

void OtrPluginObject::setNotificationEventRepository(NotificationEventRepository *notificationEventRepository)
{
    m_notificationEventRepository = notificationEventRepository;
}

void OtrPluginObject::setOtrAccountConfigurationWidgetFactory(
    OtrAccountConfigurationWidgetFactory *otrAccountConfigurationWidgetFactory)
{
    m_otrAccountConfigurationWidgetFactory = otrAccountConfigurationWidgetFactory;
}

void OtrPluginObject::setOtrBuddyConfigurationWidgetFactory(
    OtrBuddyConfigurationWidgetFactory *otrBuddyConfigurationWidgetFactory)
{
    m_otrBuddyConfigurationWidgetFactory = otrBuddyConfigurationWidgetFactory;
}

void OtrPluginObject::setOtrChatTopBarWidgetFactory(OtrChatTopBarWidgetFactory *otrChatTopBarWidgetFactory)
{
    m_otrChatTopBarWidgetFactory = otrChatTopBarWidgetFactory;
}

void OtrPluginObject::setOtrFingerprintService(OtrFingerprintService *otrFingerprintService)
{
    m_otrFingerprintService = otrFingerprintService;
}

void OtrPluginObject::setOtrInstanceTagService(OtrInstanceTagService *otrInstanceTagService)
{
    m_otrInstanceTagService = otrInstanceTagService;
}

void OtrPluginObject::setOtrIsLoggedInService(OtrIsLoggedInService *otrIsLoggedInService)
{
    m_otrIsLoggedInService = otrIsLoggedInService;
}

void OtrPluginObject::setOtrMessageService(OtrMessageService *otrMessageService)
{
    m_otrMessageService = otrMessageService;
}

void OtrPluginObject::setOtrNotifier(OtrNotifier *otrNotifier)
{
    m_otrNotifier = otrNotifier;
}

void OtrPluginObject::setOtrPeerIdentityVerificationService(
    OtrPeerIdentityVerificationService *otrPeerIdentityVerificationService)
{
    m_otrPeerIdentityVerificationService = otrPeerIdentityVerificationService;
}

void OtrPluginObject::setOtrPeerIdentityVerificationWindowRepository(
    OtrPeerIdentityVerificationWindowRepository *otrPeerIdentityVerificationWindowRepository)
{
    m_otrPeerIdentityVerificationWindowRepository = otrPeerIdentityVerificationWindowRepository;
}

void OtrPluginObject::setOtrPrivateKeyService(OtrPrivateKeyService *otrPrivateKeyService)
{
    m_otrPrivateKeyService = otrPrivateKeyService;
}

void OtrPluginObject::setOtrRawMessageTransformer(OtrRawMessageTransformer *otrRawMessageTransformer)
{
    m_otrRawMessageTransformer = otrRawMessageTransformer;
}

void OtrPluginObject::setOtrSessionService(OtrSessionService *otrSessionService)
{
    m_otrSessionService = otrSessionService;
}

void OtrPluginObject::setOtrTrustLevelService(OtrTrustLevelService *otrTrustLevelService)
{
    m_otrTrustLevelService = otrTrustLevelService;
}

void OtrPluginObject::setRawMessageTransformerService(RawMessageTransformerService *rawMessageTransformerService)
{
    m_rawMessageTransformerService = rawMessageTransformerService;
}

bool OtrPluginObject::fragmentsFixAvailable() const
{
    return (OTRL_VERSION_MAJOR > 4) || (OTRL_VERSION_MINOR > 0) || (OTRL_VERSION_SUB > 0);
}

void OtrPluginObject::init()
{
    if (!m_otrAvailable)
        throw PluginActivationErrorException("encryption_otr", tr("OTR encryption library could not be initialized"));

    m_accountConfigurationWidgetFactoryRepository->registerFactory(m_otrAccountConfigurationWidgetFactory);
    m_buddyConfigurationWidgetFactoryRepository->registerFactory(m_otrBuddyConfigurationWidgetFactory);
    m_chatTopBarWidgetFactoryRepository->registerFactory(m_otrChatTopBarWidgetFactory);

    connect(m_otrFingerprintService, SIGNAL(fingerprintsUpdated()), m_otrTrustLevelService, SLOT(updateTrustLevels()));
    m_otrFingerprintService->readFingerprints();
    m_otrInstanceTagService->readInstanceTags();

    connect(
        m_otrPeerIdentityVerificationService, SIGNAL(questionAnswerRequested(Contact, QString)),
        m_otrPeerIdentityVerificationWindowRepository,
        SLOT(showRespondQuestionAndAnswerVerificationWindow(Contact, QString)));
    connect(
        m_otrPeerIdentityVerificationService, SIGNAL(sharedSecretRequested(Contact)),
        m_otrPeerIdentityVerificationWindowRepository, SLOT(showRespondSharedSecretVerificationWindow(Contact)));

    m_otrPrivateKeyService->readPrivateKeys();

    connect(
        m_otrPrivateKeyService, SIGNAL(createPrivateKeyStarted(Account)), m_otrNotifier,
        SLOT(notifyCreatePrivateKeyStarted(Account)));
    connect(
        m_otrPrivateKeyService, SIGNAL(createPrivateKeyFinished(Account, bool)), m_otrNotifier,
        SLOT(notifyCreatePrivateKeyFinished(Account, bool)));

    connect(
        m_otrRawMessageTransformer, SIGNAL(peerEndedSession(Contact)), m_otrNotifier,
        SLOT(notifyPeerEndedSession(Contact)));

    connect(
        m_otrSessionService, SIGNAL(tryingToStartSession(Contact)), m_otrNotifier,
        SLOT(notifyTryingToStartSession(Contact)));
    connect(
        m_otrSessionService, SIGNAL(tryingToRefreshSession(Contact)), m_otrNotifier,
        SLOT(notifyTryingToRefreshSession(Contact)));
    connect(m_otrSessionService, SIGNAL(goneSecure(Contact)), m_otrNotifier, SLOT(notifyGoneSecure(Contact)));
    connect(m_otrSessionService, SIGNAL(goneInsecure(Contact)), m_otrNotifier, SLOT(notifyGoneInsecure(Contact)));
    connect(m_otrSessionService, SIGNAL(stillSecure(Contact)), m_otrNotifier, SLOT(notifyStillSecure(Contact)));

    m_otrTrustLevelService->updateTrustLevels();

    m_otrRawMessageTransformer->setEnableFragments(fragmentsFixAvailable());
    m_rawMessageTransformerService->registerTransformer(m_otrRawMessageTransformer);

    for (auto notifyEvent : m_otrNotifier->notifyEvents())
        m_notificationEventRepository->addNotificationEvent(notifyEvent);
}

void OtrPluginObject::done()
{
    if (!m_otrAvailable)
        return;

    for (auto notifyEvent : m_otrNotifier->notifyEvents())
        m_notificationEventRepository->removeNotificationEvent(notifyEvent);

    m_rawMessageTransformerService->unregisterTransformer(m_otrRawMessageTransformer);
    m_chatTopBarWidgetFactoryRepository->unregisterFactory(m_otrChatTopBarWidgetFactory);
    m_buddyConfigurationWidgetFactoryRepository->unregisterFactory(m_otrBuddyConfigurationWidgetFactory);
    m_accountConfigurationWidgetFactoryRepository->unregisterFactory(m_otrAccountConfigurationWidgetFactory);
}
