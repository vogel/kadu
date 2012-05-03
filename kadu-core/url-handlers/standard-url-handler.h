/*
 * %kadu copyright begin%
 * Copyright 2009, 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2008 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2011 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2012 Marcel Zięba (marseel@gmail.com)
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

#ifndef STANDARD_URL_HANDLER_H
#define STANDARD_URL_HANDLER_H

#include <QtCore/QRegExp>

#include "configuration/configuration-aware-object.h"
#include "configuration/configuration-holder.h"
#include "url-handler.h"

class StandardUrlHandler : public UrlHandler, public ConfigurationHolder
{
	QRegExp UrlRegExp;
	int LinkFoldTreshold;
	bool FoldLink;

protected:
	void configurationUpdated();

public:
	StandardUrlHandler();

	bool isUrlValid(const QByteArray &url);

	void convertUrlsToHtml(HtmlDocument &document, bool generateOnlyHrefAttr);

	void openUrl(const QByteArray &url, bool disableMenu = false);

	const QRegExp &urlRegExp() { return UrlRegExp; }
};

#endif // STANDARD_URL_HANDLER_H
