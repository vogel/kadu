/*
 * %kadu copyright begin%
 * Copyright 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include <QtGui/QTextDocument>

#include "configuration/configuration-file.h"
#include "os/generic/url-opener.h"

#include "standard-url-handler.h"

StandardUrlHandler::StandardUrlHandler()
{
	UrlRegExp = QRegExp("\\b(http://|https://|www\\.|ftp://)([^\\s]*)");

	configurationUpdated();
}

bool StandardUrlHandler::isUrlValid(const QByteArray &url)
{
	return UrlRegExp.exactMatch(QString::fromUtf8(url));
}

void StandardUrlHandler::convertUrlsToHtml(HtmlDocument &document, bool generateOnlyHrefAttr)
{
	for (int i = 0; i < document.countElements(); ++i)
	{
		if (document.isTagElement(i))
			continue;

		QString text = document.elementText(i);
		int index = UrlRegExp.indexIn(text);
		if (index < 0)
			continue;

		int length = UrlRegExp.matchedLength();

		QString link;
		QString displayLink = Qt::escape(text.mid(index, length));
		QString aLink = displayLink;

		if (!aLink.contains("://"))
			aLink.prepend("http://");

		if (FoldLink && (length - index > LinkFoldTreshold))
		{
			displayLink = Qt::escape(text.mid(index, index + (LinkFoldTreshold / 2)) + "..."
					+ text.mid(length - (LinkFoldTreshold / 2), LinkFoldTreshold / 2));

			if (generateOnlyHrefAttr)
				link = "<a href=\"" + aLink + "\">" + displayLink + "</a>";
			else
				// prepare string for KaduWebView::convertClipboardHtml()
				link = "<a folded=\"1\" displaystr=\"" + displayLink + "\" href=\"" + aLink + "\" title=\"" + aLink + "\">" + displayLink + "</a>";
		}
		else
		{
			if (generateOnlyHrefAttr)
				link = "<a href=\"" + aLink + "\">" + displayLink + "</a>";
			else
				link = "<a href=\"" + aLink + "\" title=\"" + aLink + "\">" + displayLink + "</a>";
		}

		document.splitElement(i, index, length);
		document.setElementValue(i, link, true);
	}
}

void StandardUrlHandler::openUrl(const QByteArray &url, bool disableMenu)
{
	Q_UNUSED(disableMenu)

	if (!url.contains("://"))
		UrlOpener::openUrl("http://" + url);
	else
		UrlOpener::openUrl(url);
}

void StandardUrlHandler::configurationUpdated()
{
	LinkFoldTreshold = config_file.readNumEntry("Chat", "LinkFoldTreshold");
	FoldLink = config_file.readBoolEntry("Chat", "FoldLink");
}
