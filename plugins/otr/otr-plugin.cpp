/*
 * %kadu copyright begin%
 * Copyright 2012 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "core/core.h"
#include "gui/widgets/account-configuration-widget-factory-repository.h"
#include "gui/widgets/chat-top-bar-widget-factory-repository.h"
#include "notify/notification-manager.h"
#include "message/message-manager.h"
#include "services/raw-message-transformer-service.h"

#include "gui/widgets/otr-account-configuration-widget-factory.h"
#include "gui/widgets/otr-chat-top-bar-widget-factory.h"
#include "gui/windows/otr-peer-identity-verification-window-factory.h"
#include "gui/windows/otr-peer-identity-verification-window-repository.h"
#include "otr-app-ops-wrapper.h"
#include "otr-context-converter.h"
#include "otr-fingerprint-service.h"
#include "otr-notifier.h"
#include "otr-op-data-factory.h"
#include "otr-peer-identity-verification-service.h"
#include "otr-private-key-service.h"
#include "otr-raw-message-transformer.h"
#include "otr-session-service.h"
#include "otr-timer.h"
#include "otr-trust-level-service.h"
#include "otr-user-state-service.h"

#include "otr-plugin.h"
#include <qvarlengtharray.h>

OtrPlugin * OtrPlugin::Instance = 0;

OtrPlugin * OtrPlugin::instance()
{
	return Instance;
}

OtrPlugin::OtrPlugin()
{
	Q_ASSERT(!Instance);
	Instance = this;

	OtrAvailable = otrl_init(OTRL_VERSION_MAJOR, OTRL_VERSION_MINOR, OTRL_VERSION_SUB) == 0;
}

OtrPlugin::~OtrPlugin()
{
	Q_ASSERT(Instance);
	Instance = 0;
}

void OtrPlugin::registerOtrAcountConfigurationWidgetFactory()
{
	AccountConfigurationWidgetFactory.reset(new OtrAccountConfigurationWidgetFactory());

	Core::instance()->accountConfigurationWidgetFactoryRepository()->registerFactory(AccountConfigurationWidgetFactory.data());
}

void OtrPlugin::unregisterOtrAcountConfigurationWidgetFactory()
{
	Core::instance()->accountConfigurationWidgetFactoryRepository()->unregisterFactory(AccountConfigurationWidgetFactory.data());

	AccountConfigurationWidgetFactory.reset();
}

void OtrPlugin::registerOtrAppOpsWrapper()
{
	AppOpsWrapper.reset(new OtrAppOpsWrapper());
}

void OtrPlugin::unregisterOtrAppOpsWrapper()
{
	AppOpsWrapper.reset();
}

void OtrPlugin::registerOtrChatTopBarWidgetFactory()
{
	ChatTopBarWidgetFactory.reset(new OtrChatTopBarWidgetFactory());

	Core::instance()->chatTopBarWidgetFactoryRepository()->registerFactory(ChatTopBarWidgetFactory.data());
}

void OtrPlugin::unregisterOtrChatTopBarWidgetFactory()
{
	Core::instance()->chatTopBarWidgetFactoryRepository()->unregisterFactory(ChatTopBarWidgetFactory.data());

	ChatTopBarWidgetFactory.reset();
}

void OtrPlugin::registerOtrContextConverter()
{
	ContextConverter.reset(new OtrContextConverter());
}

void OtrPlugin::unregisterOtrContextConverter()
{
	ContextConverter.reset();
}

void OtrPlugin::registerOtrFingerprintService()
{
	FingerprintService.reset(new OtrFingerprintService());
}

void OtrPlugin::unregisterOtrFingerprintService()
{
	FingerprintService.reset();
}

void OtrPlugin::registerOtrNotifier()
{
	Notifier.reset(new OtrNotifier());

	foreach (NotifyEvent *notifyEvent, Notifier.data()->notifyEvents())
		NotificationManager::instance()->registerNotifyEvent(notifyEvent);
}

void OtrPlugin::unregisterOtrNotifier()
{
	foreach (NotifyEvent *notifyEvent, Notifier.data()->notifyEvents())
		NotificationManager::instance()->unregisterNotifyEvent(notifyEvent);

	Notifier.reset(0);
}

void OtrPlugin::registerOtrOpDataFactory()
{
	OpDataFactory.reset(new OtrOpDataFactory());
}

void OtrPlugin::unregisterOtrOpDataFactory()
{
	OpDataFactory.reset(0);
}

void OtrPlugin::registerOtrPeerIdentityVerificationService()
{
	PeerIdentityVerificationService.reset(new OtrPeerIdentityVerificationService());
}

void OtrPlugin::unregisterOtrPeerIdentityVerificationService()
{
	PeerIdentityVerificationService.reset();
}

void OtrPlugin::registerOtrPeerIdentityVerificationWindowFactory()
{
	PeerIdentityVerificationWindowFactory.reset(new OtrPeerIdentityVerificationWindowFactory());
}

void OtrPlugin::unregisterOtrPeerIdentityVerificationWindowFactory()
{
	PeerIdentityVerificationWindowFactory.reset(0);
}

void OtrPlugin::registerOtrPeerIdentityVerificationWindowRepository()
{
	PeerIdentityVerificationWindowRepository.reset(new OtrPeerIdentityVerificationWindowRepository());
}

void OtrPlugin::unregisterOtrPeerIdentityVerificationWindowRepository()
{
	PeerIdentityVerificationWindowRepository.reset();
}

void OtrPlugin::registerOtrPrivateKeyService()
{
	PrivateKeyService.reset(new OtrPrivateKeyService());
}

void OtrPlugin::unregisterOtrPrivateKeyService()
{
	PrivateKeyService.reset();
}

void OtrPlugin::registerOtrRawMessageTransformer()
{
	RawMessageTransformer.reset(new OtrRawMessageTransformer());
	RawMessageTransformer.data()->setEnableFragments(fragmentsFixAvailable());

	Core::instance()->rawMessageTransformerService()->registerTransformer(RawMessageTransformer.data());
}

void OtrPlugin::unregisterOtrRawMessageTransformer()
{
	Core::instance()->rawMessageTransformerService()->unregisterTransformer(RawMessageTransformer.data());

	RawMessageTransformer.reset();
}

bool OtrPlugin::fragmentsFixAvailable() const
{
	return (OTRL_VERSION_MAJOR > 4) || (OTRL_VERSION_MINOR > 0) || (OTRL_VERSION_SUB > 0);
}

void OtrPlugin::registerOtrSessionService()
{
	SessionService.reset(new OtrSessionService());
}

void OtrPlugin::unregisterOtrSessionService()
{
	SessionService.reset(0);
}

void OtrPlugin::registerOtrTimer()
{
	Timer.reset(new OtrTimer());
}

void OtrPlugin::unregisterOtrTimer()
{
	Timer.reset();
}

void OtrPlugin::registerOtrTrustLevelService()
{
	TrustLevelService.reset(new OtrTrustLevelService());
}

void OtrPlugin::unregisterOtrTrustLevelService()
{
	TrustLevelService.reset();
}

void OtrPlugin::registerOtrUserStateService()
{
	UserStateService.reset(new OtrUserStateService());
}

void OtrPlugin::unregisterOtrUserStateService()
{
	UserStateService.reset(0);
}


int OtrPlugin::init(bool firstLoad)
{
	Q_UNUSED(firstLoad);

	if (!OtrAvailable)
		return 1;

	registerOtrAcountConfigurationWidgetFactory();
	registerOtrAppOpsWrapper();
	registerOtrChatTopBarWidgetFactory();
	registerOtrContextConverter();
	registerOtrFingerprintService();
	registerOtrNotifier();
	registerOtrOpDataFactory();
	registerOtrPeerIdentityVerificationService();
	registerOtrPeerIdentityVerificationWindowFactory();
	registerOtrPeerIdentityVerificationWindowRepository();
	registerOtrPrivateKeyService();
	registerOtrRawMessageTransformer();
	registerOtrSessionService();
	registerOtrTimer();
	registerOtrTrustLevelService();
	registerOtrUserStateService();

	AppOpsWrapper->setContextConverter(ContextConverter.data());
	AppOpsWrapper->setFingerprintService(FingerprintService.data());
	AppOpsWrapper->setOpDataFactory(OpDataFactory.data());
	AppOpsWrapper->setPeerIdentityVerificationService(PeerIdentityVerificationService.data());
	AppOpsWrapper->setTrustLevelService(TrustLevelService.data());
	AppOpsWrapper->setUserStateService(UserStateService.data());

	ChatTopBarWidgetFactory->setPeerIdentityVerificationWindowRepository(PeerIdentityVerificationWindowRepository.data());
	ChatTopBarWidgetFactory->setSessionService(SessionService.data());
	ChatTopBarWidgetFactory->setTrustLevelService(TrustLevelService.data());

	ContextConverter->setUserStateService(UserStateService.data());

	FingerprintService->setContextConverter(ContextConverter.data());
	FingerprintService->setUserStateService(UserStateService.data());

	connect(FingerprintService.data(), SIGNAL(fingerprintsUpdated()), TrustLevelService.data(), SLOT(updateTrustLevels()));

	FingerprintService->readFingerprints();

	OpDataFactory.data()->setAppOpsWrapper(AppOpsWrapper.data());
	OpDataFactory.data()->setPeerIdentityVerificationService(PeerIdentityVerificationService.data());
	OpDataFactory.data()->setPrivateKeyService(PrivateKeyService.data());
	OpDataFactory.data()->setSessionService(SessionService.data());

	PeerIdentityVerificationService->setAppOpsWrapper(AppOpsWrapper.data());
	PeerIdentityVerificationService->setContextConverter(ContextConverter.data());
	PeerIdentityVerificationService->setOpDataFactory(OpDataFactory.data());
	PeerIdentityVerificationService->setUserStateService(UserStateService.data());

	PeerIdentityVerificationWindowFactory->setFingerprintService(FingerprintService.data());
	PeerIdentityVerificationWindowFactory->setPeerIdentityVerificationService(PeerIdentityVerificationService.data());

	PeerIdentityVerificationWindowRepository->setPeerIdentityVerificationWindowFactory(PeerIdentityVerificationWindowFactory.data());

	PrivateKeyService->setUserStateService(UserStateService.data());
	PrivateKeyService->readPrivateKeys();

	connect(PrivateKeyService.data(), SIGNAL(createPrivateKeyStarted(Account)),
			Notifier.data(), SLOT(notifyCreatePrivateKeyStarted(Account)));
	connect(PrivateKeyService.data(), SIGNAL(createPrivateKeyFinished(Account,bool)),
			Notifier.data(), SLOT(notifyCreatePrivateKeyFinished(Account,bool)));

	RawMessageTransformer->setAppOpsWrapper(AppOpsWrapper.data());
	RawMessageTransformer->setOpDataFactory(OpDataFactory.data());
	RawMessageTransformer->setSessionService(SessionService.data());
	RawMessageTransformer->setUserStateService(UserStateService.data());

	connect(RawMessageTransformer.data(), SIGNAL(peerEndedSession(Contact)), Notifier.data(), SLOT(notifyPeerEndedSession(Contact)));

	SessionService->setAppOpsWrapper(AppOpsWrapper.data());
	SessionService->setMessageManager(MessageManager::instance());
	SessionService->setOpDataFactory(OpDataFactory.data());
	SessionService->setTrustLevelService(TrustLevelService.data());
	SessionService->setUserStateService(UserStateService.data());

	connect(SessionService.data(), SIGNAL(tryingToStartSession(Contact)), Notifier.data(), SLOT(notifyTryingToStartSession(Contact)));
	connect(SessionService.data(), SIGNAL(goneSecure(Contact)), Notifier.data(), SLOT(notifyGoneSecure(Contact)));
	connect(SessionService.data(), SIGNAL(goneInsecure(Contact)), Notifier.data(), SLOT(notifyGoneInsecure(Contact)));
	connect(SessionService.data(), SIGNAL(stillSecure(Contact)), Notifier.data(), SLOT(notifyStillSecure(Contact)));

	Timer->setAppOpsWrapper(AppOpsWrapper.data());
	Timer->setUserStateService(UserStateService.data());

	TrustLevelService->setContextConverter(ContextConverter.data());
	TrustLevelService->setUserStateService(UserStateService.data());
	TrustLevelService->updateTrustLevels();

	return 0;
}

void OtrPlugin::done()
{
	if (!OtrAvailable)
		return;

	TrustLevelService->setUserStateService(0);
	TrustLevelService->setContextConverter(ContextConverter.data());

	Timer->setUserStateService(0);
	Timer->setAppOpsWrapper(0);

	disconnect(SessionService.data(), SIGNAL(tryingToStartSession(Contact)), Notifier.data(), SLOT(notifyTryingToStartSession(Contact)));
	disconnect(SessionService.data(), SIGNAL(goneSecure(Contact)), Notifier.data(), SLOT(notifyGoneSecure(Contact)));
	disconnect(SessionService.data(), SIGNAL(goneInsecure(Contact)), Notifier.data(), SLOT(notifyGoneInsecure(Contact)));
	disconnect(SessionService.data(), SIGNAL(stillSecure(Contact)), Notifier.data(), SLOT(notifyStillSecure(Contact)));

	SessionService->setUserStateService(0);
	SessionService->setTrustLevelService(0);
	SessionService->setOpDataFactory(0);
	SessionService->setMessageManager(0);
	SessionService->setAppOpsWrapper(0);

	disconnect(RawMessageTransformer.data(), SIGNAL(peerEndedSession(Contact)), Notifier.data(), SLOT(notifyPeerEndedSession(Contact)));

	RawMessageTransformer->setUserStateService(0);
	RawMessageTransformer->setSessionService(0);
	RawMessageTransformer->setOpDataFactory(0);
	RawMessageTransformer->setAppOpsWrapper(0);

	disconnect(PrivateKeyService.data(), SIGNAL(createPrivateKeyStarted(Account)),
			   Notifier.data(), SLOT(notifyCreatePrivateKeyStarted(Account)));
	disconnect(PrivateKeyService.data(), SIGNAL(createPrivateKeyFinished(Account,bool)),
			   Notifier.data(), SLOT(notifyCreatePrivateKeyFinished(Account,bool)));

	PrivateKeyService->setUserStateService(0);

	PeerIdentityVerificationWindowFactory->setPeerIdentityVerificationService(0);
	PeerIdentityVerificationWindowFactory->setFingerprintService(0);

	PeerIdentityVerificationWindowRepository->setPeerIdentityVerificationWindowFactory(0);

	PeerIdentityVerificationService->setUserStateService(0);
	PeerIdentityVerificationService->setOpDataFactory(0);
	PeerIdentityVerificationService->setContextConverter(0);
	PeerIdentityVerificationService->setAppOpsWrapper(0);

	OpDataFactory.data()->setSessionService(0);
	OpDataFactory.data()->setPrivateKeyService(0);
	OpDataFactory.data()->setPeerIdentityVerificationService(0);
	OpDataFactory.data()->setAppOpsWrapper(0);

	disconnect(FingerprintService.data(), SIGNAL(fingerprintsUpdated()), TrustLevelService.data(), SLOT(updateTrustLevels()));

	FingerprintService->setContextConverter(0);

	ContextConverter->setUserStateService(0);

	ChatTopBarWidgetFactory->setTrustLevelService(0);
	ChatTopBarWidgetFactory->setSessionService(0);
	ChatTopBarWidgetFactory->setPeerIdentityVerificationWindowRepository(0);

	AppOpsWrapper->setUserStateService(0);
	AppOpsWrapper->setTrustLevelService(0);
	AppOpsWrapper->setPeerIdentityVerificationService(0);
	AppOpsWrapper->setOpDataFactory(0);
	AppOpsWrapper->setFingerprintService(0);
	AppOpsWrapper->setContextConverter(0);

	unregisterOtrUserStateService();
	unregisterOtrTrustLevelService();
	unregisterOtrTimer();
	unregisterOtrSessionService();
	unregisterOtrRawMessageTransformer();
	unregisterOtrPrivateKeyService();
	unregisterOtrPeerIdentityVerificationWindowRepository();
	unregisterOtrPeerIdentityVerificationWindowFactory();
	unregisterOtrPeerIdentityVerificationService();
	unregisterOtrOpDataFactory();
	unregisterOtrNotifier();
	unregisterOtrContextConverter();
	unregisterOtrChatTopBarWidgetFactory();
	unregisterOtrAppOpsWrapper();
	unregisterOtrAcountConfigurationWidgetFactory();
}

OtrTimer * OtrPlugin::otrTimer() const
{
	return Timer.data();
}

Q_EXPORT_PLUGIN2(otr, OtrPlugin)
