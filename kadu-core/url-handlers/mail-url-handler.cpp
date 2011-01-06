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

#include <QtGui/QTextDocument>

#include "os/generic/url-opener.h"

#include "mail-url-handler.h"

MailUrlHandler::MailUrlHandler()
{
	MailRegExp = QRegExp("\\b[a-zA-Z0-9_\\.\\-]+@[a-zA-Z0-9\\-\\.]+\\.[a-zA-Z]{2,4}\\b");
}

bool MailUrlHandler::isUrlValid(const QString &url)
{
	return MailRegExp.exactMatch(url);
}

void MailUrlHandler::convertUrlsToHtml(HtmlDocument &document)
{
	for (int i = 0; i < document.countElements(); ++i)
	{
		if (document.isTagElement(i))
			continue;

		QString text = document.elementText(i);
		int index = MailRegExp.indexIn(text);
		if (index < 0)
			continue;

		unsigned int length = MailRegExp.matchedLength();
		QString mail = Qt::escape(text.mid(index, length));

		document.splitElement(i, index, length);
		document.setElementValue(i, "<a href=\"mailto:" + mail + "\">" + mail + "</a>", true);
	}
}

void MailUrlHandler::openUrl(const QString &url, bool disableMenu)
{
	Q_UNUSED(disableMenu)

	UrlOpener::openEmail(url);
}
