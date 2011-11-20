/*
 * %kadu copyright begin%
 * Copyright 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2011 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#include <iris/irisnetglobal.h>

#include "protocols/protocols-manager.h"
#include "url-handlers/url-handler-manager.h"

#include "actions/jabber-actions.h"
#include "actions/jabber-protocol-menu-manager.h"
#include "certificates/trusted-certificates-manager.h"
#include "file-transfer/s5b-server-manager.h"
#include "gui/windows/main-configuration-window.h"
#include "misc/path-conversion.h"
#include "utils/vcard-factory.h"
#include "facebook-protocol-factory.h"
#include "gtalk-protocol-factory.h"
#include "jabber-id-validator.h"
#include "jabber-protocol-factory.h"
#include "jabber-url-handler.h"

#include "jabber-protocol-plugin.h"

JabberProtocolPlugin::~JabberProtocolPlugin()
{
}

int JabberProtocolPlugin::init(bool firstLoad)
{
	Q_UNUSED(firstLoad)

	if (ProtocolsManager::instance()->hasProtocolFactory("jabber")
			|| ProtocolsManager::instance()->hasProtocolFactory("gtalk")
			|| ProtocolsManager::instance()->hasProtocolFactory("facebook"))
		return 0;

	S5BServerManager::createInstance();

	JabberIdValidator::createInstance();
	VCardFactory::createInstance();

	JabberActions::registerActions();
	JabberProtocolMenuManager::createInstance();

	JabberProtocolFactory::createInstance();
	GTalkProtocolFactory::createInstance();
	FacebookProtocolFactory::createInstance();

	ProtocolsManager::instance()->registerProtocolFactory(JabberProtocolFactory::instance());
	ProtocolsManager::instance()->registerProtocolFactory(GTalkProtocolFactory::instance());
	ProtocolsManager::instance()->registerProtocolFactory(FacebookProtocolFactory::instance());

	UrlHandlerManager::instance()->registerUrlHandler("Jabber", new JabberUrlHandler());

	MainConfigurationWindow::registerUiFile(dataPath("kadu/plugins/configuration/jabber_protocol.ui"));

	return 0;
}

void JabberProtocolPlugin::done()
{
	MainConfigurationWindow::unregisterUiFile(dataPath("kadu/plugins/configuration/jabber_protocol.ui"));

	UrlHandlerManager::instance()->unregisterUrlHandler("Jabber");

	ProtocolsManager::instance()->unregisterProtocolFactory(JabberProtocolFactory::instance());
	ProtocolsManager::instance()->unregisterProtocolFactory(GTalkProtocolFactory::instance());
	ProtocolsManager::instance()->unregisterProtocolFactory(FacebookProtocolFactory::instance());

	JabberProtocolFactory::destroyInstance();
	GTalkProtocolFactory::destroyInstance();
	FacebookProtocolFactory::destroyInstance();

	JabberProtocolMenuManager::destroyInstance();
	JabberActions::unregisterActions();

	VCardFactory::destroyInstance();
	JabberIdValidator::destroyInstance();
	TrustedCertificatesManager::destroyInstance();

	S5BServerManager::destroyInstance();

	XMPP::irisNetCleanup();
}

Q_EXPORT_PLUGIN2(jabber_protocol, JabberProtocolPlugin)
