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
#include "gui/windows/otr-peer-identity-verification-window-repository.h"
#include "otr-app-ops-wrapper.h"
#include "otr-context-converter.h"
#include "otr-fingerprint-service.h"
#include "otr-notifier.h"
#include "otr-peer-identity-verification-service.h"
#include "otr-private-key-service.h"
#include "otr-raw-message-transformer.h"
#include "otr-timer.h"
#include "otr-trust-level-service.h"
#include "otr-user-state.h"

#include "otr-plugin.h"

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

void OtrPlugin::registerOtrPeerIdentityVerificationService()
{
	PeerIdentityVerificationService.reset(new OtrPeerIdentityVerificationService());
}

void OtrPlugin::unregisterOtrPeerIdentityVerificationService()
{
	PeerIdentityVerificationService.reset();
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
	registerOtrPeerIdentityVerificationService();
	registerOtrPeerIdentityVerificationWindowRepository();
	registerOtrPrivateKeyService();
	registerOtrRawMessageTransformer();
	registerOtrTimer();
	registerOtrTrustLevelService();

	AppOpsWrapper->setContextConverter(ContextConverter.data());
	AppOpsWrapper->setFingerprintService(FingerprintService.data());
	AppOpsWrapper->setMessageManager(MessageManager::instance());
	AppOpsWrapper->setPeerIdentityVerificationService(PeerIdentityVerificationService.data());
	AppOpsWrapper->setTrustLevelService(TrustLevelService.data());
	AppOpsWrapper->setUserState(&UserState);

	connect(AppOpsWrapper.data(), SIGNAL(tryToStartSession(Chat)), Notifier.data(), SLOT(notifyTryToStartSession(Chat)));
	connect(AppOpsWrapper.data(), SIGNAL(peerClosedSession(Chat)), Notifier.data(), SLOT(notifyPeerClosedSession(Chat)));
	connect(AppOpsWrapper.data(), SIGNAL(goneSecure(Chat)), Notifier.data(), SLOT(notifyGoneSecure(Chat)));
	connect(AppOpsWrapper.data(), SIGNAL(goneInsecure(Chat)), Notifier.data(), SLOT(notifyGoneInsecure(Chat)));
	connect(AppOpsWrapper.data(), SIGNAL(stillSecure(Chat)), Notifier.data(), SLOT(notifyStillSecure(Chat)));

	ChatTopBarWidgetFactory->setAppOpsWrapper(AppOpsWrapper.data());
	ChatTopBarWidgetFactory->setPeerIdentityVerificationWindowRepository(PeerIdentityVerificationWindowRepository.data());
	ChatTopBarWidgetFactory->setTrustLevelService(TrustLevelService.data());

	ContextConverter->setUserState(&UserState);

	FingerprintService->setContextConverter(ContextConverter.data());
	FingerprintService->setUserState(&UserState);

	connect(FingerprintService.data(), SIGNAL(fingerprintsUpdated()), TrustLevelService.data(), SLOT(updateTrustLevels()));

	FingerprintService->readFingerprints();

	PeerIdentityVerificationService->setAppOpsWrapper(AppOpsWrapper.data());

	PeerIdentityVerificationWindowRepository->setAppOpsWrapper(AppOpsWrapper.data());
	PeerIdentityVerificationWindowRepository->setFingerprintService(FingerprintService.data());
	PeerIdentityVerificationWindowRepository->setPeerIdentityVerificationService(PeerIdentityVerificationService.data());

	PrivateKeyService->setUserState(&UserState);
	PrivateKeyService->readPrivateKeys();

	connect(PrivateKeyService.data(), SIGNAL(createPrivateKeyStarted(Account)),
			Notifier.data(), SLOT(notifyCreatePrivateKeyStarted(Account)));
	connect(PrivateKeyService.data(), SIGNAL(createPrivateKeyFinished(Account,bool)),
			Notifier.data(), SLOT(notifyCreatePrivateKeyFinished(Account,bool)));

	RawMessageTransformer->setOtrAppOpsWrapper(AppOpsWrapper.data());
	RawMessageTransformer->setOtrPrivateKeyService(PrivateKeyService.data());
	RawMessageTransformer->setUserState(&UserState);

	Timer->setOtrAppOpsWrapper(AppOpsWrapper.data());
	Timer->setUserState(&UserState);

	TrustLevelService->setUserState(&UserState);
	TrustLevelService->setContextConverter(ContextConverter.data());
	TrustLevelService->updateTrustLevels();

	return 0;
}

void OtrPlugin::done()
{
	if (!OtrAvailable)
		return;

	TrustLevelService->setContextConverter(ContextConverter.data());
	TrustLevelService->setUserState(&UserState);

	Timer->setUserState(0);
	Timer->setOtrAppOpsWrapper(0);

	RawMessageTransformer->setUserState(0);
	RawMessageTransformer->setOtrPrivateKeyService(0);
	RawMessageTransformer->setOtrAppOpsWrapper(0);

	disconnect(PrivateKeyService.data(), SIGNAL(createPrivateKeyStarted(Account)),
			   Notifier.data(), SLOT(notifyCreatePrivateKeyStarted(Account)));
	disconnect(PrivateKeyService.data(), SIGNAL(createPrivateKeyFinished(Account,bool)),
			   Notifier.data(), SLOT(notifyCreatePrivateKeyFinished(Account,bool)));

	PrivateKeyService->setUserState(0);

	PeerIdentityVerificationWindowRepository->setPeerIdentityVerificationService(0);
	PeerIdentityVerificationWindowRepository->setFingerprintService(0);
	PeerIdentityVerificationWindowRepository->setAppOpsWrapper(0);

	PeerIdentityVerificationService->setAppOpsWrapper(0);

	disconnect(FingerprintService.data(), SIGNAL(fingerprintsUpdated()), TrustLevelService.data(), SLOT(updateTrustLevels()));

	FingerprintService->setContextConverter(0);

	ContextConverter->setUserState(0);

	ChatTopBarWidgetFactory->setTrustLevelService(0);
	ChatTopBarWidgetFactory->setPeerIdentityVerificationWindowRepository(0);
	ChatTopBarWidgetFactory->setAppOpsWrapper(0);

	disconnect(AppOpsWrapper.data(), SIGNAL(tryToStartSession(Chat)), Notifier.data(), SLOT(notifyTryToStartSession(Chat)));
	disconnect(AppOpsWrapper.data(), SIGNAL(peerClosedSession(Chat)), Notifier.data(), SLOT(notifyPeerClosedSession(Chat)));
	disconnect(AppOpsWrapper.data(), SIGNAL(goneSecure(Chat)), Notifier.data(), SLOT(notifyGoneSecure(Chat)));
	disconnect(AppOpsWrapper.data(), SIGNAL(goneInsecure(Chat)), Notifier.data(), SLOT(notifyGoneInsecure(Chat)));
	disconnect(AppOpsWrapper.data(), SIGNAL(stillSecure(Chat)), Notifier.data(), SLOT(notifyStillSecure(Chat)));

	AppOpsWrapper->setUserState(0);
	AppOpsWrapper->setTrustLevelService(0);
	AppOpsWrapper->setPeerIdentityVerificationService(0);
	AppOpsWrapper->setMessageManager(0);
	AppOpsWrapper->setFingerprintService(0);
	AppOpsWrapper->setContextConverter(0);

	unregisterOtrTrustLevelService();
	unregisterOtrTimer();
	unregisterOtrRawMessageTransformer();
	unregisterOtrPrivateKeyService();
	unregisterOtrPeerIdentityVerificationWindowRepository();
	unregisterOtrPeerIdentityVerificationService();
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

Q_EXPORT_PLUGIN2(encryption_ng_otr, OtrPlugin)
