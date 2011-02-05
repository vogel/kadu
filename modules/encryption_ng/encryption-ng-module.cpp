/*
 * Copyright 2007, 2008, 2009 Tomasz Kazmierczak
 * %kadu copyright begin%
 * Copyright 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include <QtCore/QCoreApplication>
#include <QtCore/QtGlobal>
#include <QtGui/QApplication>
#include <QtCrypto>

#include "gui/windows/message-dialog.h"
#include "misc/path-conversion.h"
#include "exports.h"

#include "configuration/encryption-ng-configuration.h"
#include "keys/keys-manager.h"
#include "notify/encryption-ng-notification.h"
#include "encryption-actions.h"
#include "encryption-manager.h"
#include "encryption-ng-configuration-ui-handler.h"
#include "encryption-provider-manager.h"

namespace EncryptionNg
{
	static QCA::Initializer *InitObject;
}

extern "C" KADU_EXPORT int encryption_ng_init(bool firstLoad)
{
	Q_UNUSED(firstLoad)

	EncryptionNg::InitObject = new QCA::Initializer();

	if (!QCA::isSupported("pkey") ||
			!QCA::PKey::supportedIOTypes().contains(QCA::PKey::RSA) ||
			!QCA::isSupported("sha1"))
	{
		MessageDialog::exec("dialog-error", QApplication::tr("Encryption"),
				QApplication::tr("The QCA OSSL plugin for libqca2 is not present!"));

		delete EncryptionNg::InitObject;
		EncryptionNg::InitObject = 0;
		qRemovePostRoutine(QCA::deinit);

		return -1;
	}

	EncryptionNgNotification::registerNotifications();
	EncryptionNgConfiguration::createInstance();
	EncryptionNgConfigurationUiHandler::registerConfigurationUi();

	EncryptionProviderManager::createInstance();
	EncryptionActions::registerActions();
	EncryptionManager::createInstance();

	return 0;
}

extern "C" KADU_EXPORT void encryption_ng_close()
{
	EncryptionManager::destroyInstance();
	EncryptionActions::unregisterActions();
	EncryptionProviderManager::destroyInstance();

	EncryptionNgConfigurationUiHandler::unregisterConfigurationUi();
	EncryptionNgConfiguration::destroyInstance();
	EncryptionNgNotification::unregisterNotifications();

	KeysManager::destroyInstance();

	delete EncryptionNg::InitObject;
	EncryptionNg::InitObject = 0;
	qRemovePostRoutine(QCA::deinit);
}
