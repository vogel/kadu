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

#ifndef OTR_PLUGIN_H
#define OTR_PLUGIN_H

#include <QtCore/QObject>

#include "plugins/generic-plugin.h"

class OtrAccountConfigurationWidgetFactory;
class OtrAppOpsWrapper;
class OtrChatTopBarWidgetFactory;
class OtrContextConverter;
class OtrErrorMessageService;
class OtrFingerprintService;
class OtrInstanceTagService;
class OtrIsLoggedInService;
class OtrMessageService;
class OtrNotifier;
class OtrOpDataFactory;
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

class OtrPlugin : public QObject, public GenericPlugin
{
	Q_OBJECT
	Q_INTERFACES(GenericPlugin)

	bool OtrAvailable;

	QScopedPointer<OtrAccountConfigurationWidgetFactory> AccountConfigurationWidgetFactory;
	QScopedPointer<OtrAppOpsWrapper> AppOpsWrapper;
	QScopedPointer<OtrChatTopBarWidgetFactory> ChatTopBarWidgetFactory;
	QScopedPointer<OtrContextConverter> ContextConverter;
	QScopedPointer<OtrErrorMessageService> ErrorMessageService;
	QScopedPointer<OtrFingerprintService> FingerprintService;
	QScopedPointer<OtrInstanceTagService> InstanceTagService;
	QScopedPointer<OtrIsLoggedInService> IsLoggedInService;
	QScopedPointer<OtrMessageService> MessageService;
	QScopedPointer<OtrNotifier> Notifier;
	QScopedPointer<OtrOpDataFactory> OpDataFactory;
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

	void registerOtrAcountConfigurationWidgetFactory();
	void unregisterOtrAcountConfigurationWidgetFactory();

	void registerOtrAppOpsWrapper();
	void unregisterOtrAppOpsWrapper();

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

	void registerOtrMessageService();
	void unregisterOtrMessageService();

	void registerOtrNotifier();
	void unregisterOtrNotifier();

	void registerOtrOpDataFactory();
	void unregisterOtrOpDataFactory();

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

	virtual int init(bool firstLoad);
	virtual void done();

};

#endif // OTR_PLUGIN_H
