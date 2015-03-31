/*
 * %kadu copyright begin%
 * Copyright 2011, 2012, 2013 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2011, 2012, 2013, 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "core/application.h"
#include "protocols/protocols-manager.h"
#include "url-handlers/url-handler-manager.h"

#include "actions/jabber-actions.h"
#include "actions/jabber-protocol-menu-manager.h"
#include "core/core.h"
#include "dom/dom-processor-service.h"
#include "gui/windows/main-configuration-window.h"
#include "misc/paths-provider.h"
#include "facebook-depreceated-message.h"
#include "gtalk-protocol-factory.h"
#include "jabber-id-validator.h"
#include "jabber-protocol-factory.h"
#include "jabber-url-dom-visitor-provider.h"
#include "jabber-url-handler.h"

#include "jabber-protocol-plugin.h"

JabberProtocolPlugin::~JabberProtocolPlugin()
{
}

bool JabberProtocolPlugin::init(bool firstLoad)
{
	Q_UNUSED(firstLoad)

	if (ProtocolsManager::instance()->hasProtocolFactory("jabber")
			|| ProtocolsManager::instance()->hasProtocolFactory("gtalk"))
		return true;

	FacebookDepreceatedMessage::createInstance();

	JabberIdValidator::createInstance();

	JabberActions::registerActions();
	JabberProtocolMenuManager::createInstance();

	JabberProtocolFactory::createInstance();
	GTalkProtocolFactory::createInstance();

	ProtocolsManager::instance()->registerProtocolFactory(JabberProtocolFactory::instance());
	ProtocolsManager::instance()->registerProtocolFactory(GTalkProtocolFactory::instance());

	UrlHandlerManager::instance()->registerUrlHandler("Jabber", new JabberUrlHandler());

	// install before mail handler
	UrlDomVisitorProvider = new JabberUrlDomVisitorProvider();
	Core::instance()->domProcessorService()->registerVisitorProvider(UrlDomVisitorProvider, 200);

	return true;
}

void JabberProtocolPlugin::done()
{
	UrlHandlerManager::instance()->unregisterUrlHandler("Jabber");

	Core::instance()->domProcessorService()->unregisterVisitorProvider(UrlDomVisitorProvider);
	delete UrlDomVisitorProvider;
	UrlDomVisitorProvider = 0;

	ProtocolsManager::instance()->unregisterProtocolFactory(JabberProtocolFactory::instance());
	ProtocolsManager::instance()->unregisterProtocolFactory(GTalkProtocolFactory::instance());

	JabberProtocolFactory::destroyInstance();
	GTalkProtocolFactory::destroyInstance();

	JabberProtocolMenuManager::destroyInstance();
	JabberActions::unregisterActions();

	JabberIdValidator::destroyInstance();

	FacebookDepreceatedMessage::destroyInstance();
}

Q_EXPORT_PLUGIN2(jabber_protocol, JabberProtocolPlugin)

#include "moc_jabber-protocol-plugin.cpp"
