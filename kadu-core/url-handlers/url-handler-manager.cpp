/*
 * %kadu copyright begin%
 * Copyright 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2010, 2011 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#include <QtXml/QDomDocument>


#include "core/core.h"

#include "dom/dom-processor-service.h"
#include "dom/ignore-links-dom-visitor.h"
#include "url-handlers/mail-url-handler.h"
#include "url-handlers/standard-url-expander.h"
#include "url-handlers/standard-url-expander-configurator.h"
#include "url-handlers/standard-url-handler.h"

#include "url-handler-manager.h"
#include "mail-url-expander.h"

UrlHandlerManager * UrlHandlerManager::Instance = 0;

UrlHandlerManager * UrlHandlerManager::instance()
{
	if (0 == Instance)
		Instance = new UrlHandlerManager();

	return Instance;
}

UrlHandlerManager::UrlHandlerManager()
{
	StandardExpander = new StandardUrlExpander(QRegExp("\\b(http://|https://|www\\.|ftp://)([^\\s]*)"));
	StandardIgnoreLinksVisitor = new IgnoreLinksDomVisitor(StandardExpander);
	StandardConfigurator = new StandardUrlExpanderConfigurator();
	StandardConfigurator->setStandardUrlExpander(StandardExpander);

	Core::instance()->domProcessorService()->registerVisitor(StandardIgnoreLinksVisitor, 0);

	MailExpander = new MailUrlExpander(QRegExp("\\b[a-zA-Z0-9_\\.\\-]+@[a-zA-Z0-9\\-\\.]+\\.[a-zA-Z]{2,4}\\b"));
	MailIgnoreLinksVisitor = new IgnoreLinksDomVisitor(MailExpander);

	Core::instance()->domProcessorService()->registerVisitor(MailIgnoreLinksVisitor, 500);

	// NOTE: StandardUrlHandler has to be the first one to fix bug #1894
	standardUrlHandler = new StandardUrlHandler();
	registerUrlHandler("Standard", standardUrlHandler);

	mailUrlHandler = new MailUrlHandler();
	registerUrlHandler("Mail", mailUrlHandler);
}

UrlHandlerManager::~UrlHandlerManager()
{
	Core::instance()->domProcessorService()->unregisterVisitor(StandardIgnoreLinksVisitor);

	delete StandardConfigurator;
	StandardConfigurator = 0;

	delete StandardIgnoreLinksVisitor;
	StandardIgnoreLinksVisitor = 0;

	delete StandardExpander;
	StandardExpander = 0;

	Core::instance()->domProcessorService()->unregisterVisitor(MailIgnoreLinksVisitor);

	delete MailIgnoreLinksVisitor;
	MailIgnoreLinksVisitor = 0;

	delete MailExpander;
	MailExpander = 0;

	qDeleteAll(RegisteredHandlersByPriority);
	RegisteredHandlersByPriority.clear();
	RegisteredHandlers.clear();
}

void UrlHandlerManager::registerUrlHandler(const QString &name, UrlHandler *handler)
{
	if (0 != handler && !RegisteredHandlers.contains(name))
	{
		RegisteredHandlers[name] = handler;
		// TODO: some day it might be useful to add priority to UrlHandler class like in StatusChanger
		RegisteredHandlersByPriority.append(handler);
	}
}

void UrlHandlerManager::unregisterUrlHandler(const QString &name)
{
	if (RegisteredHandlers.contains(name))
	{
		UrlHandler *handler = RegisteredHandlers.take(name);
		RegisteredHandlersByPriority.removeAll(handler);
		delete handler;
	}
}

void UrlHandlerManager::openUrl(const QByteArray &url, bool disableMenu)
{
	foreach (UrlHandler *handler, RegisteredHandlersByPriority)
	{
		if (handler->isUrlValid(url))
		{
			handler->openUrl(url, disableMenu);
			return;
		}
	}
}

const QRegExp & UrlHandlerManager::mailRegExp()
{
	return mailUrlHandler->mailRegExp();
}

const QRegExp & UrlHandlerManager::urlRegExp()
{
	return standardUrlHandler->urlRegExp();
}
