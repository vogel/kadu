/*
 * %kadu copyright begin%
 * Copyright 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include <QtGui/QTextDocument>

#include "os/generic/url-opener.h"

#include "mail-url-handler.h"

MailUrlHandler::MailUrlHandler()
{
	MailRegExp = QRegExp("\\b[a-zA-Z0-9_\\.\\-]+@[a-zA-Z0-9\\-\\.]+\\.[a-zA-Z]{2,4}\\b");
}

bool MailUrlHandler::isUrlValid(const QByteArray &url)
{
	return MailRegExp.exactMatch(QString::fromUtf8(url));
}

void MailUrlHandler::convertUrlsToHtml(HtmlDocument &document, bool generateOnlyHrefAttr)
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

		QString anchor;
		if (generateOnlyHrefAttr)
			anchor = "<a href=\"mailto:" + mail + "\">" + mail + "</a>";
		else
			anchor = "<a href=\"mailto:" + mail + "\" title=\"" + mail +"\">" + mail + "</a>";

		document.setElementValue(i, anchor, true);
	}
}

void MailUrlHandler::openUrl(const QByteArray &url, bool disableMenu)
{
	Q_UNUSED(disableMenu)

	UrlOpener::openEmail(url);
}
