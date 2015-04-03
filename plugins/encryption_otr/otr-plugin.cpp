/*
 * %kadu copyright begin%
 * Copyright 2014, 2015 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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
#include "notification/notification-event-repository.h"
#include "notification/notification-manager.h"
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
#include "otr-module.h"
#include "otr-notifier.h"
#include "otr-op-data-factory.h"
#include "otr-path-service.h"
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

bool OtrPlugin::fragmentsFixAvailable() const
{
	return (OTRL_VERSION_MAJOR > 4) || (OTRL_VERSION_MINOR > 0) || (OTRL_VERSION_SUB > 0);
}

bool OtrPlugin::init(bool firstLoad)
{
	Q_UNUSED(firstLoad);

	if (!OtrAvailable)
		return false;

	auto modules = std::vector<std::unique_ptr<injeqt::module>>{};
	modules.emplace_back(make_unique<OtrModule>());

	m_injector = make_unique<injeqt::injector>(std::move(modules));

	Core::instance()->accountConfigurationWidgetFactoryRepository()->registerFactory(m_injector->get<OtrAccountConfigurationWidgetFactory>());
	Core::instance()->buddyConfigurationWidgetFactoryRepository()->registerFactory(m_injector->get<OtrBuddyConfigurationWidgetFactory>());
	Core::instance()->chatTopBarWidgetFactoryRepository()->registerFactory(m_injector->get<OtrChatTopBarWidgetFactory>());

	connect(m_injector->get<OtrFingerprintService>(), SIGNAL(fingerprintsUpdated()), m_injector->get<OtrTrustLevelService>(), SLOT(updateTrustLevels()));
	m_injector->get<OtrFingerprintService>()->readFingerprints();
	m_injector->get<OtrInstanceTagService>()->readInstanceTags();

	m_injector->get<OtrIsLoggedInService>()->setContactManager(ContactManager::instance());
	m_injector->get<OtrMessageService>()->setMessageManager(MessageManager::instance());
	m_injector->get<OtrNotifier>()->setChatWidgetRepository(Core::instance()->chatWidgetRepository());

	connect(m_injector->get<OtrPeerIdentityVerificationService>(), SIGNAL(questionAnswerRequested(Contact,QString)),
			m_injector->get<OtrPeerIdentityVerificationWindowRepository>(), SLOT(showRespondQuestionAndAnswerVerificationWindow(Contact,QString)));
	connect(m_injector->get<OtrPeerIdentityVerificationService>(), SIGNAL(sharedSecretRequested(Contact)),
			m_injector->get<OtrPeerIdentityVerificationWindowRepository>(), SLOT(showRespondSharedSecretVerificationWindow(Contact)));

	m_injector->get<OtrPrivateKeyService>()->readPrivateKeys();

	connect(m_injector->get<OtrPrivateKeyService>(), SIGNAL(createPrivateKeyStarted(Account)),
			m_injector->get<OtrNotifier>(), SLOT(notifyCreatePrivateKeyStarted(Account)));
	connect(m_injector->get<OtrPrivateKeyService>(), SIGNAL(createPrivateKeyFinished(Account,bool)),
			m_injector->get<OtrNotifier>(), SLOT(notifyCreatePrivateKeyFinished(Account,bool)));

	connect(m_injector->get<OtrRawMessageTransformer>(), SIGNAL(peerEndedSession(Contact)),
			m_injector->get<OtrNotifier>(), SLOT(notifyPeerEndedSession(Contact)));

	connect(m_injector->get<OtrSessionService>(), SIGNAL(tryingToStartSession(Contact)),
			m_injector->get<OtrNotifier>(), SLOT(notifyTryingToStartSession(Contact)));
	connect(m_injector->get<OtrSessionService>(), SIGNAL(tryingToRefreshSession(Contact)),
			m_injector->get<OtrNotifier>(), SLOT(notifyTryingToRefreshSession(Contact)));
	connect(m_injector->get<OtrSessionService>(), SIGNAL(goneSecure(Contact)),
			m_injector->get<OtrNotifier>(), SLOT(notifyGoneSecure(Contact)));
	connect(m_injector->get<OtrSessionService>(), SIGNAL(goneInsecure(Contact)),
			m_injector->get<OtrNotifier>(), SLOT(notifyGoneInsecure(Contact)));
	connect(m_injector->get<OtrSessionService>(), SIGNAL(stillSecure(Contact)),
			m_injector->get<OtrNotifier>(), SLOT(notifyStillSecure(Contact)));

	m_injector->get<OtrTrustLevelService>()->updateTrustLevels();
	m_injector->get<OtrSessionService>()->setMessageManager(MessageManager::instance());

	m_injector->get<OtrRawMessageTransformer>()->setEnableFragments(fragmentsFixAvailable());
	Core::instance()->rawMessageTransformerService()->registerTransformer(m_injector->get<OtrRawMessageTransformer>());

	for (auto notifyEvent : m_injector->get<OtrNotifier>()->notifyEvents())
		Core::instance()->notificationEventRepository()->addNotificationEvent(notifyEvent);

	return true;
}

void OtrPlugin::done()
{
	if (!OtrAvailable)
		return;

	for (auto notifyEvent : m_injector->get<OtrNotifier>()->notifyEvents())
		Core::instance()->notificationEventRepository()->removeNotificationEvent(notifyEvent);

	Core::instance()->rawMessageTransformerService()->unregisterTransformer(m_injector->get<OtrRawMessageTransformer>());
	Core::instance()->chatTopBarWidgetFactoryRepository()->unregisterFactory(m_injector->get<OtrChatTopBarWidgetFactory>());
	Core::instance()->buddyConfigurationWidgetFactoryRepository()->unregisterFactory(m_injector->get<OtrBuddyConfigurationWidgetFactory>());
	Core::instance()->accountConfigurationWidgetFactoryRepository()->unregisterFactory(m_injector->get<OtrAccountConfigurationWidgetFactory>());

	m_injector.reset();
}

Q_EXPORT_PLUGIN2(otr, OtrPlugin)
