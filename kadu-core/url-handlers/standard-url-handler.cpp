/*
 * %kadu copyright begin%
 * Copyright 2010 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2010, 2011 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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
	// TODO: WTF? fix it!!!!!!!!!!!!!!!!!!!!!!!
	UrlRegExp = QRegExp("\\b(http://|https://|www\\.|ftp://|ftp\\.|sftp://|smb://|file:/|rsync://|svn://|svn\\+ssh://)(\\w|[a-zA-Z0-9ąśężźćółń\\*\\-\\._/~?=&#\\+%\\(\\)\\[\\]:;,!@\\\\])*");

	configurationUpdated();
}

bool StandardUrlHandler::isUrlValid(const QByteArray &url)
{
	return UrlRegExp.exactMatch(QString::fromUtf8(url));
}

void StandardUrlHandler::convertUrlsToHtml(HtmlDocument &document)
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

		aLink.replace("%20", "%2520"); // Opera's bug workaround - allows opening links with spaces
		if (!aLink.contains("://"))
			aLink.prepend("http://");

		if ((length - index > LinkFoldTreshold) && FoldLink)
			displayLink = Qt::escape(text.mid(index, index + (LinkFoldTreshold / 2)) + "..."
					+ text.mid(length - (LinkFoldTreshold / 2), LinkFoldTreshold / 2));

		link = "<a href=\"" + aLink + "\" title=\"" + aLink + "\">" + displayLink + "</a>";

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
