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

#ifndef OTR_PLUGIN_H
#define OTR_PLUGIN_H

#include <QtCore/QObject>

#include "plugin/plugin-root-component.h"

class OtrAccountConfigurationWidgetFactory;
class OtrAppOpsService;
class OtrBuddyConfigurationWidgetFactory;
class OtrChatTopBarWidgetFactory;
class OtrContextConverter;
class OtrErrorMessageService;
class OtrFingerprintService;
class OtrInstanceTagService;
class OtrIsLoggedInService;
class OtrMessageEventService;
class OtrMessageService;
class OtrNotifier;
class OtrOpDataFactory;
class OtrPathService;
class OtrPeerIdentityVerificationService;
class OtrPeerIdentityVerificationWindowFactory;
class OtrPeerIdentityVerificationWindowRepository;
class OtrPrivateKeyService;
class OtrPolicyService;
class OtrRawMessageTransformer;
class OtrSessionService;
class OtrTimerService;
class OtrTrustLevelService;
class OtrUserStateService;

class OtrPlugin : public QObject, public PluginRootComponent
{
	Q_OBJECT
	Q_INTERFACES(PluginRootComponent)
	Q_PLUGIN_METADATA(IID "im.kadu.PluginRootComponent")

	bool OtrAvailable;

	QScopedPointer<OtrAccountConfigurationWidgetFactory> AccountConfigurationWidgetFactory;
	QScopedPointer<OtrAppOpsService> AppOpsService;
	QScopedPointer<OtrBuddyConfigurationWidgetFactory> BuddyConfigurationWidgetFactory;
	QScopedPointer<OtrChatTopBarWidgetFactory> ChatTopBarWidgetFactory;
	QScopedPointer<OtrContextConverter> ContextConverter;
	QScopedPointer<OtrErrorMessageService> ErrorMessageService;
	QScopedPointer<OtrFingerprintService> FingerprintService;
	QScopedPointer<OtrInstanceTagService> InstanceTagService;
	QScopedPointer<OtrIsLoggedInService> IsLoggedInService;
	QScopedPointer<OtrMessageEventService> MessageEventService;
	QScopedPointer<OtrMessageService> MessageService;
	QScopedPointer<OtrNotifier> Notifier;
	QScopedPointer<OtrOpDataFactory> OpDataFactory;
	QScopedPointer<OtrPathService> PathService;
	QScopedPointer<OtrPeerIdentityVerificationService> PeerIdentityVerificationService;
	QScopedPointer<OtrPeerIdentityVerificationWindowFactory> PeerIdentityVerificationWindowFactory;
	QScopedPointer<OtrPeerIdentityVerificationWindowRepository> PeerIdentityVerificationWindowRepository;
	QScopedPointer<OtrPolicyService> PolicyService;
	QScopedPointer<OtrPrivateKeyService> PrivateKeyService;
	QScopedPointer<OtrRawMessageTransformer> RawMessageTransformer;
	QScopedPointer<OtrSessionService> SessionService;
	QScopedPointer<OtrTimerService> TimerService;
	QScopedPointer<OtrTrustLevelService> TrustLevelService;
	QScopedPointer<OtrUserStateService> UserStateService;

	void registerOtrAccountConfigurationWidgetFactory();
	void unregisterOtrAccountConfigurationWidgetFactory();

	void registerOtrAppOpsService();
	void unregisterOtrAppOpsService();

	void registerOtrBuddyConfigurationWidgetFactory();
	void unregisterOtrBuddyConfigurationWidgetFactory();

	void registerOtrChatTopBarWidgetFactory();
	void unregisterOtrChatTopBarWidgetFactory();

	void registerOtrContextConverter();
	void unregisterOtrContextConverter();

	void registerOtrErrorMessageService();
	void unregisterOtrErrorMessageService();

	void registerOtrFingerprintService();
	void unregisterOtrFingerprintService();

	void registerOtrInstanceTagService();
	void unregisterOtrInstanceTagService();

	void registerOtrIsLoggedInService();
	void unregisterOtrIsLoggedInService();

	void registerOtrMessageEventService();
	void unregisterOtrMessageEventService();

	void registerOtrMessageService();
	void unregisterOtrMessageService();

	void registerOtrNotifier();
	void unregisterOtrNotifier();

	void registerOtrOpDataFactory();
	void unregisterOtrOpDataFactory();

	void registerOtrPathService();
	void unregisterOtrPathService();

	void registerOtrPeerIdentityVerificationService();
	void unregisterOtrPeerIdentityVerificationService();

	void registerOtrPeerIdentityVerificationWindowFactory();
	void unregisterOtrPeerIdentityVerificationWindowFactory();

	void registerOtrPeerIdentityVerificationWindowRepository();
	void unregisterOtrPeerIdentityVerificationWindowRepository();

	void registerOtrPolicyService();
	void unregisterOtrPolicyService();

	void registerOtrPrivateKeyService();
	void unregisterOtrPrivateKeyService();

	void registerOtrRawMessageTransformer();
	void unregisterOtrRawMessageTransformer();
	bool fragmentsFixAvailable() const;

	void registerOtrSessionService();
	void unregisterOtrSessionService();

	void registerOtrTimer();
	void unregisterOtrTimer();

	void registerOtrTrustLevelService();
	void unregisterOtrTrustLevelService();

	void registerOtrUserStateService();
	void unregisterOtrUserStateService();

public:
	explicit OtrPlugin();
	virtual ~OtrPlugin();

	virtual bool init(bool firstLoad);
	virtual void done();

};

#endif // OTR_PLUGIN_H
