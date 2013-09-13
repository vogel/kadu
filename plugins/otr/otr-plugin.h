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

#ifndef OTR_PLUGIN_H
#define OTR_PLUGIN_H

#include <QtCore/QObject>

#include "plugins/generic-plugin.h"

#include "otr-user-state.h"

class OtrAccountConfigurationWidgetFactory;
class OtrAppOpsWrapper;
class OtrChatTopBarWidgetFactory;
class OtrNotifier;
class OtrPeerIdentityVerifier;
class OtrPrivateKeyService;
class OtrRawMessageTransformer;
class OtrTimer;

class OtrPlugin : public QObject, public GenericPlugin
{
	Q_OBJECT
	Q_INTERFACES(GenericPlugin)

	static OtrPlugin *Instance; // ugly, but required by otr unfortunately
	bool OtrAvailable;
	OtrUserState UserState;

	QScopedPointer<OtrAccountConfigurationWidgetFactory> AccountConfigurationWidgetFactory;
	QScopedPointer<OtrAppOpsWrapper> AppOpsWrapper;
	QScopedPointer<OtrChatTopBarWidgetFactory> ChatTopBarWidgetFactory;
	QScopedPointer<OtrNotifier> Notifier;
	QScopedPointer<OtrPeerIdentityVerifier> PeerIdentityVerifier;
	QScopedPointer<OtrPrivateKeyService> PrivateKeyService;
	QScopedPointer<OtrRawMessageTransformer> RawMessageTransformer;
	QScopedPointer<OtrTimer> Timer;

	void registerOtrAcountConfigurationWidgetFactory();
	void unregisterOtrAcountConfigurationWidgetFactory();

	void registerOtrAppOpsWrapper();
	void unregisterOtrAppOpsWrapper();

	void registerOtrChatTopBarWidgetFactory();
	void unregisterOtrChatTopBarWidgetFactory();

	void registerOtrNotifier();
	void unregisterOtrNotifier();

	void registerOtrPeerIdentityVerifier();
	void unregisterOtrPeerIdentityVerifier();

	void registerOtrPrivateKeyService();
	void unregisterOtrPrivateKeyService();

	void registerOtrRawMessageTransformer();
	void unregisterOtrRawMessageTransformer();
	bool fragmentsFixAvailable() const;

	void registerOtrTimer();
	void unregisterOtrTimer();

public:
	static OtrPlugin * instance();

	explicit OtrPlugin();
	virtual ~OtrPlugin();

	virtual int init(bool firstLoad);
	virtual void done();

	OtrTimer * otrTimer() const;

};

#endif // OTR_PLUGIN_H
