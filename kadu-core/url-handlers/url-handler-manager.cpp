/*
 * %kadu copyright begin%
 * Copyright 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2011, 2012, 2013 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2010, 2011, 2012 Bartosz Brachaczek (b.brachaczek@gmail.com)
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
#include "gui/services/clipboard-html-transformer-service.h"
#include "url-handlers/mail-url-dom-visitor-provider.h"
#include "url-handlers/mail-url-handler.h"
#include "url-handlers/standard-url-dom-visitor-provider.h"
#include "url-handlers/standard-url-handler.h"
#include "url-handlers/url-clipboard-html-transformer.h"

#include "url-handler-manager.h"

UrlHandlerManager * UrlHandlerManager::Instance = 0;

UrlHandlerManager * UrlHandlerManager::instance()
{
	if (0 == Instance)
		Instance = new UrlHandlerManager();

	return Instance;
}

UrlHandlerManager::UrlHandlerManager()
{
	StandardUrlVisitorProvider = new StandardUrlDomVisitorProvider();
	Core::instance()->domProcessorService()->registerVisitorProvider(StandardUrlVisitorProvider, 0);

	MailUrlVisitorProvider = new MailUrlDomVisitorProvider();
	Core::instance()->domProcessorService()->registerVisitorProvider(MailUrlVisitorProvider, 500);

	// NOTE: StandardUrlHandler has to be the first one to fix bug #1894
	standardUrlHandler = new StandardUrlHandler();
	registerUrlHandler("Standard", standardUrlHandler);

	mailUrlHandler = new MailUrlHandler();
	registerUrlHandler("Mail", mailUrlHandler);
	registerUrlClipboardTransformer();
}

UrlHandlerManager::~UrlHandlerManager()
{
	unregisterUrlClipboardTransformer();

	Core::instance()->domProcessorService()->unregisterVisitorProvider(StandardUrlVisitorProvider);
	delete StandardUrlVisitorProvider;
	StandardUrlVisitorProvider = 0;

	Core::instance()->domProcessorService()->unregisterVisitorProvider(MailUrlVisitorProvider);
	delete MailUrlVisitorProvider;
	MailUrlVisitorProvider = 0;

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

void UrlHandlerManager::registerUrlClipboardTransformer()
{
	ClipboardTransformer.reset(new UrlClipboardHtmlTransformer());
	Core::instance()->clipboardHtmlTransformerService()->registerTransformer(ClipboardTransformer.data());
}

void UrlHandlerManager::unregisterUrlClipboardTransformer()
{
	Core::instance()->clipboardHtmlTransformerService()->unregisterTransformer(ClipboardTransformer.data());
	ClipboardTransformer.reset();
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
