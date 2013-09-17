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
#include "otr-app-ops-wrapper.h"
#include "otr-context-converter.h"
#include "otr-fingerprint-extractor.h"
#include "otr-fingerprint-service.h"
#include "otr-notifier.h"
#include "otr-peer-identity-verifier.h"
#include "otr-private-key-service.h"
#include "otr-raw-message-transformer.h"
#include "otr-timer.h"
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

void OtrPlugin::registerOtrFingerprintExtractor()
{
	FingerprintExtractor.reset(new OtrFingerprintExtractor());
}

void OtrPlugin::registerOtrFingerprintService()
{
	FingerprintService.reset(new OtrFingerprintService());
}

void OtrPlugin::unregisterOtrFingerprintService()
{
	FingerprintService.reset();
}

void OtrPlugin::unregisterOtrFingerprintExtractor()
{
	FingerprintExtractor.reset();
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

void OtrPlugin::registerOtrPeerIdentityVerificationWindowFactory()
{
	PeerIdentityVerificationWindowFactory.reset(new OtrPeerIdentityVerificationWindowFactory());
}

void OtrPlugin::unregisterOtrPeerIdentityVerificationWindowFactory()
{
	PeerIdentityVerificationWindowFactory.reset();
}

void OtrPlugin::registerOtrPeerIdentityVerifier()
{
	PeerIdentityVerifier.reset(new OtrPeerIdentityVerifier());
}

void OtrPlugin::unregisterOtrPeerIdentityVerifier()
{
	PeerIdentityVerifier.reset();
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

int OtrPlugin::init(bool firstLoad)
{
	Q_UNUSED(firstLoad);

	if (!OtrAvailable)
		return 1;

	registerOtrAcountConfigurationWidgetFactory();
	registerOtrAppOpsWrapper();
	registerOtrChatTopBarWidgetFactory();
	registerOtrContextConverter();
	registerOtrFingerprintExtractor();
	registerOtrFingerprintService();
	registerOtrNotifier();
	registerOtrPeerIdentityVerificationWindowFactory();
	registerOtrPeerIdentityVerifier();
	registerOtrPrivateKeyService();
	registerOtrRawMessageTransformer();
	registerOtrTimer();

	AppOpsWrapper->setContextConverter(ContextConverter.data());
	AppOpsWrapper->setFingerprintService(FingerprintService.data());
	AppOpsWrapper->setMessageManager(MessageManager::instance());
	AppOpsWrapper->setUserState(&UserState);

	ChatTopBarWidgetFactory->setOtrAppOpsWrapper(AppOpsWrapper.data());
	ChatTopBarWidgetFactory->setPeerIdentityVerifier(PeerIdentityVerifier.data());

	ContextConverter->setUserState(&UserState);

	FingerprintExtractor->setContextConverter(ContextConverter.data());
	FingerprintExtractor->setUserState(&UserState);

	FingerprintService->setContextConverter(ContextConverter.data());
	FingerprintService->setUserState(&UserState);
	FingerprintService->readFingerprints();

	PeerIdentityVerificationWindowFactory->setFingerprintExtractor(FingerprintExtractor.data());
	PeerIdentityVerificationWindowFactory->setFingerprintTrust(FingerprintService.data());

	PeerIdentityVerifier->setOtrPeerIdentityVerificationWindowFactory(PeerIdentityVerificationWindowFactory.data());

	connect(AppOpsWrapper.data(), SIGNAL(tryToStartSession(Chat)), Notifier.data(), SLOT(notifyTryToStartSession(Chat)));
	connect(AppOpsWrapper.data(), SIGNAL(peerClosedSession(Chat)), Notifier.data(), SLOT(notifyPeerClosedSession(Chat)));
	connect(AppOpsWrapper.data(), SIGNAL(goneSecure(Chat)), Notifier.data(), SLOT(notifyGoneSecure(Chat)));
	connect(AppOpsWrapper.data(), SIGNAL(goneInsecure(Chat)), Notifier.data(), SLOT(notifyGoneInsecure(Chat)));
	connect(AppOpsWrapper.data(), SIGNAL(stillSecure(Chat)), Notifier.data(), SLOT(notifyStillSecure(Chat)));

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

	return 0;
}

void OtrPlugin::done()
{
	if (!OtrAvailable)
		return;

	Timer->setUserState(0);
	Timer->setOtrAppOpsWrapper(0);

	RawMessageTransformer->setUserState(0);
	RawMessageTransformer->setOtrPrivateKeyService(0);
	RawMessageTransformer->setOtrAppOpsWrapper(0);

	PrivateKeyService->setUserState(0);

	disconnect(AppOpsWrapper.data(), SIGNAL(tryToStartSession(Chat)), Notifier.data(), SLOT(notifyTryToStartSession(Chat)));
	disconnect(AppOpsWrapper.data(), SIGNAL(peerClosedSession(Chat)), Notifier.data(), SLOT(notifyPeerClosedSession(Chat)));
	disconnect(AppOpsWrapper.data(), SIGNAL(goneSecure(Chat)), Notifier.data(), SLOT(notifyGoneSecure(Chat)));
	disconnect(AppOpsWrapper.data(), SIGNAL(goneInsecure(Chat)), Notifier.data(), SLOT(notifyGoneInsecure(Chat)));
	disconnect(AppOpsWrapper.data(), SIGNAL(stillSecure(Chat)), Notifier.data(), SLOT(notifyStillSecure(Chat)));
	disconnect(AppOpsWrapper.data(), SIGNAL(contextListUpdated()), ChatTopBarWidgetFactory.data(), SLOT(updateTrustStatuses()));

	PeerIdentityVerifier->setOtrPeerIdentityVerificationWindowFactory(0);

	PeerIdentityVerificationWindowFactory->setFingerprintTrust(0);
	PeerIdentityVerificationWindowFactory->setFingerprintExtractor(0);

	FingerprintService->setContextConverter(0);

	FingerprintExtractor->setUserState(0);
	FingerprintExtractor->setContextConverter(0);

	ContextConverter->setUserState(0);

	ChatTopBarWidgetFactory->setPeerIdentityVerifier(0);
	ChatTopBarWidgetFactory->setOtrAppOpsWrapper(0);

	AppOpsWrapper->setUserState(0);
	AppOpsWrapper->setMessageManager(0);
	AppOpsWrapper->setFingerprintService(0);
	AppOpsWrapper->setContextConverter(0);

	unregisterOtrTimer();
	unregisterOtrRawMessageTransformer();
	unregisterOtrPrivateKeyService();
	unregisterOtrPeerIdentityVerifier();
	unregisterOtrPeerIdentityVerificationWindowFactory();
	unregisterOtrNotifier();
	unregisterOtrFingerprintExtractor();
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
