/*
 * Copyright 2007, 2008, 2009 Tomasz Kazmierczak
 * %kadu copyright begin%
 * Copyright 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009, 2012 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2004 Adrian Smarzewski (adrian@kadu.net)
 * Copyright 2007, 2008, 2009, 2010, 2011, 2012, 2013 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2010, 2011, 2012, 2013 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#include <QtCore/QCoreApplication>
#include <QtCore/QtGlobal>
#include <QtWidgets/QApplication>
#include <QtCrypto>

#include "gui/windows/message-dialog.h"
#include "misc/paths-provider.h"
#include "exports.h"

#include "keys/keys-manager.h"
#include "notify/encryption-ng-notification.h"
#include "encryption-actions.h"
#include "encryption-manager.h"
#include "encryption-provider-manager.h"

#include "encryption-ng-plugin.h"

EncryptionNgPlugin::~EncryptionNgPlugin()
{

}

bool EncryptionNgPlugin::init(bool firstLoad)
{
	Q_UNUSED(firstLoad)

	if (!QCA::isSupported("pkey") ||
			!QCA::PKey::supportedIOTypes().contains(QCA::PKey::RSA) ||
			!QCA::isSupported("sha1"))
	{
		MessageDialog::show(KaduIcon("dialog-error"), tr("Encryption"),
				tr("The QCA OSSL plugin for libqca2 is not present!"));

		return false;
	}

	EncryptionNgNotification::registerNotifications();

	EncryptionProviderManager::createInstance();
	EncryptionActions::registerActions();
	EncryptionManager::createInstance();

	return true;
}

void EncryptionNgPlugin::done()
{
	EncryptionManager::destroyInstance();
	EncryptionActions::unregisterActions();
	EncryptionProviderManager::destroyInstance();

	EncryptionNgNotification::unregisterNotifications();

	KeysManager::destroyInstance();
}

Q_EXPORT_PLUGIN2(encryption_ng_plugin, EncryptionNgPlugin)

#include "moc_encryption-ng-plugin.cpp"
