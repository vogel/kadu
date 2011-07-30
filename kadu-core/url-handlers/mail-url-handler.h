/*
 * %kadu copyright begin%
 * Copyright 2010 Piotr Galiszewski (piotr.galiszewski@kadu.im)
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

#ifndef MAIL_URL_HANDLER_H
#define MAIL_URL_HANDLER_H

#include <QtCore/QRegExp>

#include "url-handler.h"

class MailUrlHandler : public UrlHandler
{
	QRegExp MailRegExp;

public:
	MailUrlHandler();

	bool isUrlValid(const QByteArray &url);

	void convertUrlsToHtml(HtmlDocument &document, bool generateOnlyHrefAttr);

	void openUrl(const QByteArray &url, bool disableMenu);

	const QRegExp & mailRegExp() { return MailRegExp; }
};

#endif // MAIL_URL_HANDLER_H
