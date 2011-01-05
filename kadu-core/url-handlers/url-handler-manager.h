/*
 * %kadu copyright begin%
 * Copyright 2010 Piotr Galiszewski (piotrgaliszewski@gmail.com)
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

#ifndef URL_HANDLER_MANAGER_H
#define URL_HANDLER_MANAGER_H

#include <QtCore/QList>
#include <QtCore/QMap>

#include "html_document.h"
#include "exports.h"

class MailUrlHandler;
class StandardUrlHandler;
class UrlHandler;

class KADUAPI UrlHandlerManager
{
	Q_DISABLE_COPY(UrlHandlerManager)

	static UrlHandlerManager *Instance;
	UrlHandlerManager();

	QMap<QString, UrlHandler *> RegisteredHandlers;
	QList<UrlHandler *> RegisteredHandlersByPriority;

	MailUrlHandler *mailUrlHandler;
	StandardUrlHandler *standardUrlHandler;

public:
	static KADUAPI UrlHandlerManager * instance();
	~UrlHandlerManager();

	void registerUrlHandler(const QString &name, UrlHandler *handler);
	void unregisterUrlHandler(const QString &name);

	void convertAllUrls(HtmlDocument &document);

	void openUrl(const QString &url, bool disableMenu = false);

	// TODO:
	//for mail validation:
	const QRegExp & mailRegExp();

	//for link validation
	const QRegExp & urlRegExp();
};

#endif // URL_HANDLER_MANAGER_H
