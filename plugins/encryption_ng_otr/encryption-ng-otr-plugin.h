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

#ifndef ENCRYPTION_NG_OTR_PLUGIN_H
#define ENCRYPTION_NG_OTR_PLUGIN_H

#include <QtCore/QObject>

#include "plugins/generic-plugin.h"

#include "encryption-ng-otr-user-state.h"

class EncryptionNgOtrAccountConfigurationWidgetFactory;
class EncryptionNgOtrAppOpsWrapper;
class EncryptionNgOtrNotifier;
class EncryptionNgOtrPrivateKeyService;
class EncryptionNgOtrRawMessageTransformer;
class EncryptionNgOtrTimer;

class EncryptionNgOtrPlugin : public QObject, public GenericPlugin
{
	Q_OBJECT
	Q_INTERFACES(GenericPlugin)

	static EncryptionNgOtrPlugin *Instance; // ugly, but required by otr unfortunately
	bool OtrAvailable;
	EncryptionNgOtrUserState OtrUserState;

	QScopedPointer<EncryptionNgOtrAccountConfigurationWidgetFactory> OtrAccountConfigurationWidgetFactory;
	QScopedPointer<EncryptionNgOtrAppOpsWrapper> OtrAppOpsWrapper;
	QScopedPointer<EncryptionNgOtrNotifier> OtrNotifier;
	QScopedPointer<EncryptionNgOtrPrivateKeyService> OtrPrivateKeyService;
	QScopedPointer<EncryptionNgOtrRawMessageTransformer> OtrRawMessageTransformer;
	QScopedPointer<EncryptionNgOtrTimer> OtrTimer;

	void registerOtrAcountConfigurationWidgetFactory();
	void unregisterOtrAcountConfigurationWidgetFactory();

	void registerOtrAppOpsWrapper();
	void unregisterOtrAppOpsWrapper();

	void registerOtrNotifier();
	void unregisterOtrNotifier();

	void registerOtrPrivateKeyService();
	void unregisterOtrPrivateKeyService();

	void registerOtrRawMessageTransformer();
	void unregisterOtrRawMessageTransformer();

	void registerOtrTimer();
	void unregisterOtrTimer();

public:
	static EncryptionNgOtrPlugin * instance();

	explicit EncryptionNgOtrPlugin();
	virtual ~EncryptionNgOtrPlugin();

	virtual int init(bool firstLoad);
	virtual void done();

	EncryptionNgOtrTimer * otrTimer() const;

};

#endif // ENCRYPTION_NG_OTR_PLUGIN_H
