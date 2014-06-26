/*
 * %kadu copyright begin%
 * Copyright 2013 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "contacts/contact-manager.h"
#include "core/core.h"
#include "gui/widgets/account-configuration-widget-factory-repository.h"
#include "gui/widgets/buddy-configuration-widget-factory-repository.h"
#include "gui/widgets/chat-top-bar-widget-factory-repository.h"
#include "message/message-manager.h"
#include "notify/notification-manager.h"
#include "services/raw-message-transformer-service.h"

#include "gui/widgets/otr-account-configuration-widget-factory.h"
#include "gui/widgets/otr-buddy-configuration-widget-factory.h"
#include "gui/widgets/otr-chat-top-bar-widget-factory.h"
#include "gui/windows/otr-peer-identity-verification-window-factory.h"
#include "gui/windows/otr-peer-identity-verification-window-repository.h"
#include "otr-app-ops-service.h"
#include "otr-context-converter.h"
#include "otr-error-message-service.h"
#include "otr-error-message-service.h"
#include "otr-fingerprint-service.h"
#include "otr-instance-tag-service.h"
#include "otr-is-logged-in-service.h"
#include "otr-message-event-service.h"
#include "otr-message-service.h"
#include "otr-notifier.h"
#include "otr-op-data-factory.h"
#include "otr-peer-identity-verification-service.h"
#include "otr-policy-service.h"
#include "otr-private-key-service.h"
#include "otr-raw-message-transformer.h"
#include "otr-session-service.h"
#include "otr-timer-service.h"
#include "otr-trust-level-service.h"
#include "otr-user-state-service.h"

#include "otr-plugin.h"

OtrPlugin::OtrPlugin()
{
	OtrAvailable = otrl_init(OTRL_VERSION_MAJOR, OTRL_VERSION_MINOR, OTRL_VERSION_SUB) == 0;
}

OtrPlugin::~OtrPlugin()
{
}

void OtrPlugin::registerOtrAccountConfigurationWidgetFactory()
{
	AccountConfigurationWidgetFactory.reset(new OtrAccountConfigurationWidgetFactory());
}

void OtrPlugin::unregisterOtrAccountConfigurationWidgetFactory()
{
	AccountConfigurationWidgetFactory.reset();
}

void OtrPlugin::registerOtrAppOpsService()
{
	AppOpsService.reset(new OtrAppOpsService());
}

void OtrPlugin::unregisterOtrAppOpsService()
{
	AppOpsService.reset();
}

void OtrPlugin::registerOtrBuddyConfigurationWidgetFactory()
{
	BuddyConfigurationWidgetFactory.reset(new OtrBuddyConfigurationWidgetFactory());
}

void OtrPlugin::unregisterOtrBuddyConfigurationWidgetFactory()
{
	BuddyConfigurationWidgetFactory.reset();
}

void OtrPlugin::registerOtrChatTopBarWidgetFactory()
{
	ChatTopBarWidgetFactory.reset(new OtrChatTopBarWidgetFactory());
}

void OtrPlugin::unregisterOtrChatTopBarWidgetFactory()
{
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

void OtrPlugin::registerOtrErrorMessageService()
{
	ErrorMessageService.reset(new OtrErrorMessageService());
}

void OtrPlugin::unregisterOtrErrorMessageService()
{
	ErrorMessageService.reset();
}

void OtrPlugin::registerOtrFingerprintService()
{
	FingerprintService.reset(new OtrFingerprintService());
}

void OtrPlugin::unregisterOtrFingerprintService()
{
	FingerprintService.reset();
}

void OtrPlugin::registerOtrInstanceTagService()
{
	InstanceTagService.reset(new OtrInstanceTagService());
}

void OtrPlugin::registerOtrIsLoggedInService()
{
	IsLoggedInService.reset(new OtrIsLoggedInService());
}

void OtrPlugin::unregisterOtrIsLoggedInService()
{
	IsLoggedInService.reset();
}

void OtrPlugin::unregisterOtrInstanceTagService()
{
	InstanceTagService.reset();
}

void OtrPlugin::registerOtrMessageEventService()
{
	MessageEventService.reset(new OtrMessageEventService());
}

void OtrPlugin::unregisterOtrMessageEventService()
{
	MessageEventService.reset();
}

void OtrPlugin::registerOtrMessageService()
{
	MessageService.reset(new OtrMessageService());
}

void OtrPlugin::unregisterOtrMessageService()
{
	MessageService.reset();
}

void OtrPlugin::registerOtrNotifier()
{
	Notifier.reset(new OtrNotifier());

	foreach (NotifyEvent *notifyEvent, Notifier->notifyEvents())
		NotificationManager::instance()->registerNotifyEvent(notifyEvent);
}

void OtrPlugin::unregisterOtrNotifier()
{
	foreach (NotifyEvent *notifyEvent, Notifier->notifyEvents())
		NotificationManager::instance()->unregisterNotifyEvent(notifyEvent);

	Notifier.reset();
}

void OtrPlugin::registerOtrOpDataFactory()
{
	OpDataFactory.reset(new OtrOpDataFactory());
}

void OtrPlugin::unregisterOtrOpDataFactory()
{
	OpDataFactory.reset();
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
	PeerIdentityVerificationWindowFactory.reset();
}

void OtrPlugin::registerOtrPeerIdentityVerificationWindowRepository()
{
	PeerIdentityVerificationWindowRepository.reset(new OtrPeerIdentityVerificationWindowRepository());
}

void OtrPlugin::unregisterOtrPeerIdentityVerificationWindowRepository()
{
	PeerIdentityVerificationWindowRepository.reset();
}

void OtrPlugin::registerOtrPolicyService()
{
	PolicyService.reset(new OtrPolicyService());
}

void OtrPlugin::unregisterOtrPolicyService()
{
	PolicyService.reset();
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
	RawMessageTransformer->setEnableFragments(fragmentsFixAvailable());

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
	SessionService.reset();
}

void OtrPlugin::registerOtrTimer()
{
	TimerService.reset(new OtrTimerService());
}

void OtrPlugin::unregisterOtrTimer()
{
	TimerService.reset();
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
	UserStateService.reset();
}

bool OtrPlugin::init(bool firstLoad)
{
	Q_UNUSED(firstLoad);

	if (!OtrAvailable)
		return false;

	registerOtrAccountConfigurationWidgetFactory();
	registerOtrAppOpsService();
	registerOtrBuddyConfigurationWidgetFactory();
	registerOtrChatTopBarWidgetFactory();
	registerOtrContextConverter();
	registerOtrErrorMessageService();
	registerOtrFingerprintService();
	registerOtrInstanceTagService();
	registerOtrIsLoggedInService();
	registerOtrMessageEventService();
	registerOtrMessageService();
	registerOtrNotifier();
	registerOtrOpDataFactory();
	registerOtrPeerIdentityVerificationService();
	registerOtrPeerIdentityVerificationWindowFactory();
	registerOtrPeerIdentityVerificationWindowRepository();
	registerOtrPolicyService();
	registerOtrPrivateKeyService();
	registerOtrRawMessageTransformer();
	registerOtrSessionService();
	registerOtrTimer();
	registerOtrTrustLevelService();
	registerOtrUserStateService();

	AccountConfigurationWidgetFactory->setPolicyService(PolicyService.data());
	Core::instance()->accountConfigurationWidgetFactoryRepository()->registerFactory(AccountConfigurationWidgetFactory.data());

	BuddyConfigurationWidgetFactory->setPolicyService(PolicyService.data());
	Core::instance()->buddyConfigurationWidgetFactoryRepository()->registerFactory(BuddyConfigurationWidgetFactory.data());

	ChatTopBarWidgetFactory->setPeerIdentityVerificationWindowRepository(PeerIdentityVerificationWindowRepository.data());
	ChatTopBarWidgetFactory->setSessionService(SessionService.data());
	ChatTopBarWidgetFactory->setTrustLevelService(TrustLevelService.data());
	Core::instance()->chatTopBarWidgetFactoryRepository()->registerFactory(ChatTopBarWidgetFactory.data());

	ContextConverter->setUserStateService(UserStateService.data());

	FingerprintService->setContextConverter(ContextConverter.data());
	FingerprintService->setUserStateService(UserStateService.data());
	connect(FingerprintService.data(), SIGNAL(fingerprintsUpdated()), TrustLevelService.data(), SLOT(updateTrustLevels()));
	FingerprintService->readFingerprints();

	InstanceTagService->setUserStateService(UserStateService.data());
	InstanceTagService->readInstanceTags();

	IsLoggedInService->setContactManager(ContactManager::instance());

	MessageService->setMessageManager(MessageManager::instance());

	Notifier->setChatWidgetRepository(Core::instance()->chatWidgetRepository());

	OpDataFactory->setErrorMessageService(ErrorMessageService.data());
	OpDataFactory->setFingerprintService(FingerprintService.data());
	OpDataFactory->setInstanceTagService(InstanceTagService.data());
	OpDataFactory->setIsLoggedInService(IsLoggedInService.data());
	OpDataFactory->setMessageEventService(MessageEventService.data());
	OpDataFactory->setMessageService(MessageService.data());
	OpDataFactory->setPeerIdentityVerificationService(PeerIdentityVerificationService.data());
	OpDataFactory->setPolicyService(PolicyService.data());
	OpDataFactory->setPrivateKeyService(PrivateKeyService.data());
	OpDataFactory->setSessionService(SessionService.data());
	OpDataFactory->setTimerService(TimerService.data());
	OpDataFactory->setTrustLevelService(TrustLevelService.data());

	PeerIdentityVerificationService->setAppOpsService(AppOpsService.data());
	PeerIdentityVerificationService->setContextConverter(ContextConverter.data());
	PeerIdentityVerificationService->setOpDataFactory(OpDataFactory.data());
	PeerIdentityVerificationService->setUserStateService(UserStateService.data());

	connect(PeerIdentityVerificationService.data(), SIGNAL(questionAnswerRequested(Contact,QString)),
			PeerIdentityVerificationWindowRepository.data(), SLOT(showRespondQuestionAndAnswerVerificationWindow(Contact,QString)));
	connect(PeerIdentityVerificationService.data(), SIGNAL(sharedSecretRequested(Contact)),
			PeerIdentityVerificationWindowRepository.data(), SLOT(showRespondSharedSecretVerificationWindow(Contact)));

	PeerIdentityVerificationWindowFactory->setFingerprintService(FingerprintService.data());
	PeerIdentityVerificationWindowFactory->setPeerIdentityVerificationService(PeerIdentityVerificationService.data());
	PeerIdentityVerificationWindowFactory->setTrustLevelService(TrustLevelService.data());

	PeerIdentityVerificationWindowRepository->setPeerIdentityVerificationWindowFactory(PeerIdentityVerificationWindowFactory.data());

	PrivateKeyService->setUserStateService(UserStateService.data());
	PrivateKeyService->readPrivateKeys();

	connect(PrivateKeyService.data(), SIGNAL(createPrivateKeyStarted(Account)),
			Notifier.data(), SLOT(notifyCreatePrivateKeyStarted(Account)));
	connect(PrivateKeyService.data(), SIGNAL(createPrivateKeyFinished(Account,bool)),
			Notifier.data(), SLOT(notifyCreatePrivateKeyFinished(Account,bool)));

	RawMessageTransformer->setAppOpsService(AppOpsService.data());
	RawMessageTransformer->setOpDataFactory(OpDataFactory.data());
	RawMessageTransformer->setSessionService(SessionService.data());
	RawMessageTransformer->setUserStateService(UserStateService.data());

	connect(RawMessageTransformer.data(), SIGNAL(peerEndedSession(Contact)), Notifier.data(), SLOT(notifyPeerEndedSession(Contact)));

	SessionService->setAppOpsService(AppOpsService.data());
	SessionService->setMessageManager(MessageManager::instance());
	SessionService->setOpDataFactory(OpDataFactory.data());
	SessionService->setPolicyService(PolicyService.data());
	SessionService->setTrustLevelService(TrustLevelService.data());
	SessionService->setUserStateService(UserStateService.data());

	connect(SessionService.data(), SIGNAL(tryingToStartSession(Contact)), Notifier.data(), SLOT(notifyTryingToStartSession(Contact)));
	connect(SessionService.data(), SIGNAL(tryingToRefreshSession(Contact)), Notifier.data(), SLOT(notifyTryingToRefreshSession(Contact)));
	connect(SessionService.data(), SIGNAL(goneSecure(Contact)), Notifier.data(), SLOT(notifyGoneSecure(Contact)));
	connect(SessionService.data(), SIGNAL(goneInsecure(Contact)), Notifier.data(), SLOT(notifyGoneInsecure(Contact)));
	connect(SessionService.data(), SIGNAL(stillSecure(Contact)), Notifier.data(), SLOT(notifyStillSecure(Contact)));

	TimerService->setAppOpsService(AppOpsService.data());
	TimerService->setOpDataFactory(OpDataFactory.data());
	TimerService->setUserStateService(UserStateService.data());

	TrustLevelService->setContextConverter(ContextConverter.data());
	TrustLevelService->setUserStateService(UserStateService.data());
	TrustLevelService->updateTrustLevels();

	return true;
}

void OtrPlugin::done()
{
	if (!OtrAvailable)
		return;

	TrustLevelService->setUserStateService(0);
	TrustLevelService->setContextConverter(ContextConverter.data());

	TimerService->setUserStateService(0);
	TimerService->setOpDataFactory(0);
	TimerService->setAppOpsService(0);

	disconnect(SessionService.data(), SIGNAL(tryingToStartSession(Contact)), Notifier.data(), SLOT(notifyTryingToStartSession(Contact)));
	disconnect(SessionService.data(), SIGNAL(tryingToRefreshSession(Contact)), Notifier.data(), SLOT(notifyTryingToRefreshSession(Contact)));
	disconnect(SessionService.data(), SIGNAL(goneSecure(Contact)), Notifier.data(), SLOT(notifyGoneSecure(Contact)));
	disconnect(SessionService.data(), SIGNAL(goneInsecure(Contact)), Notifier.data(), SLOT(notifyGoneInsecure(Contact)));
	disconnect(SessionService.data(), SIGNAL(stillSecure(Contact)), Notifier.data(), SLOT(notifyStillSecure(Contact)));

	SessionService->setUserStateService(0);
	SessionService->setTrustLevelService(0);
	SessionService->setPolicyService(0);
	SessionService->setOpDataFactory(0);
	SessionService->setMessageManager(0);
	SessionService->setAppOpsService(0);

	disconnect(RawMessageTransformer.data(), SIGNAL(peerEndedSession(Contact)), Notifier.data(), SLOT(notifyPeerEndedSession(Contact)));

	RawMessageTransformer->setUserStateService(0);
	RawMessageTransformer->setSessionService(0);
	RawMessageTransformer->setOpDataFactory(0);
	RawMessageTransformer->setAppOpsService(0);

	disconnect(PrivateKeyService.data(), SIGNAL(createPrivateKeyStarted(Account)),
			   Notifier.data(), SLOT(notifyCreatePrivateKeyStarted(Account)));
	disconnect(PrivateKeyService.data(), SIGNAL(createPrivateKeyFinished(Account,bool)),
			   Notifier.data(), SLOT(notifyCreatePrivateKeyFinished(Account,bool)));

	PrivateKeyService->setUserStateService(0);

	PeerIdentityVerificationWindowFactory->setTrustLevelService(0);
	PeerIdentityVerificationWindowFactory->setPeerIdentityVerificationService(0);
	PeerIdentityVerificationWindowFactory->setFingerprintService(0);

	PeerIdentityVerificationWindowRepository->setPeerIdentityVerificationWindowFactory(0);

	disconnect(PeerIdentityVerificationService.data(), SIGNAL(questionAnswerRequested(Contact,QString)),
			   PeerIdentityVerificationWindowRepository.data(), SLOT(showRespondQuestionAndAnswerVerificationWindow(Contact,QString)));
	disconnect(PeerIdentityVerificationService.data(), SIGNAL(sharedSecretRequested(Contact)),
			   PeerIdentityVerificationWindowRepository.data(), SLOT(showRespondSharedSecretVerificationWindow(Contact)));

	PeerIdentityVerificationService->setUserStateService(0);
	PeerIdentityVerificationService->setOpDataFactory(0);
	PeerIdentityVerificationService->setContextConverter(0);
	PeerIdentityVerificationService->setAppOpsService(0);

	OpDataFactory->setTrustLevelService(0);
	OpDataFactory->setTimerService(0);
	OpDataFactory->setSessionService(0);
	OpDataFactory->setPrivateKeyService(0);
	OpDataFactory->setPolicyService(0);
	OpDataFactory->setPeerIdentityVerificationService(0);
	OpDataFactory->setMessageService(0);
	OpDataFactory->setMessageEventService(0);
	OpDataFactory->setIsLoggedInService(0);
	OpDataFactory->setInstanceTagService(0);
	OpDataFactory->setFingerprintService(0);
	OpDataFactory->setErrorMessageService(0);

	MessageService->setMessageManager(0);

	IsLoggedInService->setContactManager(0);

	InstanceTagService->writeInstanceTags();
	InstanceTagService->setUserStateService(0);

	disconnect(FingerprintService.data(), SIGNAL(fingerprintsUpdated()), TrustLevelService.data(), SLOT(updateTrustLevels()));

	FingerprintService->setContextConverter(0);

	ContextConverter->setUserStateService(0);

	ChatTopBarWidgetFactory->setTrustLevelService(0);
	ChatTopBarWidgetFactory->setSessionService(0);
	ChatTopBarWidgetFactory->setPeerIdentityVerificationWindowRepository(0);
	Core::instance()->chatTopBarWidgetFactoryRepository()->unregisterFactory(ChatTopBarWidgetFactory.data());

	BuddyConfigurationWidgetFactory->setPolicyService(0);
	Core::instance()->buddyConfigurationWidgetFactoryRepository()->unregisterFactory(BuddyConfigurationWidgetFactory.data());

	AccountConfigurationWidgetFactory->setPolicyService(0);
	Core::instance()->accountConfigurationWidgetFactoryRepository()->unregisterFactory(AccountConfigurationWidgetFactory.data());

	unregisterOtrUserStateService();
	unregisterOtrTrustLevelService();
	unregisterOtrTimer();
	unregisterOtrSessionService();
	unregisterOtrRawMessageTransformer();
	unregisterOtrPrivateKeyService();
	unregisterOtrPolicyService();
	unregisterOtrPeerIdentityVerificationWindowRepository();
	unregisterOtrPeerIdentityVerificationWindowFactory();
	unregisterOtrPeerIdentityVerificationService();
	unregisterOtrOpDataFactory();
	unregisterOtrNotifier();
	unregisterOtrMessageService();
	unregisterOtrMessageEventService();
	unregisterOtrIsLoggedInService();
	unregisterOtrInstanceTagService();
	unregisterOtrFingerprintService();
	unregisterOtrErrorMessageService();
	unregisterOtrContextConverter();
	unregisterOtrChatTopBarWidgetFactory();
	unregisterOtrBuddyConfigurationWidgetFactory();
	unregisterOtrAppOpsService();
	unregisterOtrAccountConfigurationWidgetFactory();
}

Q_EXPORT_PLUGIN2(otr, OtrPlugin)
