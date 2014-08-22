/*
 * %kadu copyright begin%
 * Copyright 2012 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2012, 2013, 2014 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2013 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#include <QtWebKit/QWebFrame>

#include "gui/widgets/webkit-messages-view/webkit-messages-view.h"

#include "web-view-highlighter.h"

WebViewHighlighter::WebViewHighlighter(WebkitMessagesView *parent) :
		QObject(parent), AutoUpdate(false)
{
}

WebViewHighlighter::~WebViewHighlighter()
{
}

WebkitMessagesView * WebViewHighlighter::chatMessagesView() const
{
	return static_cast<WebkitMessagesView *>(parent());
}

void WebViewHighlighter::setAutoUpdate(const bool autoUpdate)
{
	if (AutoUpdate == autoUpdate)
		return;

	// we might assume that when any message is added to web view its size changes
	// unfortunately contentChanged() does not work when content is updated by javascript
	// inside webkit instance

	if (AutoUpdate)
		disconnect(chatMessagesView()->page()->mainFrame(), SIGNAL(contentsSizeChanged(QSize)),
		           this, SLOT(updateHighlighting()));

	AutoUpdate = autoUpdate;

	if (AutoUpdate)
		connect(chatMessagesView()->page()->mainFrame(), SIGNAL(contentsSizeChanged(QSize)),
		        this, SLOT(updateHighlighting()));
}

void WebViewHighlighter::setHighlight(const QString &highlightString)
{
	if (HighlightString == highlightString)
		return;

	clearHighlighting();
	HighlightString = highlightString;
	updateHighlighting();
}

void WebViewHighlighter::updateHighlighting()
{
	if (HighlightString.isEmpty())
		return;

	bool found = false;
	// reset to first occurence
	chatMessagesView()->findText(QString(), QWebPage::FindWrapsAroundDocument);
	chatMessagesView()->findText(HighlightString, QWebPage::FindWrapsAroundDocument);

	// highlight all other
	found = chatMessagesView()->findText(HighlightString, QWebPage::HighlightAllOccurrences);

	emit somethingFound(found);
}

void WebViewHighlighter::clearHighlighting()
{
	chatMessagesView()->findText(QString(), QWebPage::HighlightAllOccurrences);

	emit somethingFound(true);
}

void WebViewHighlighter::selectNext(const QString &select)
{
	bool found = chatMessagesView()->findText(select, QWebPage::FindWrapsAroundDocument);
	chatMessagesView()->updateAtBottom();

	emit somethingFound(found);
}

void WebViewHighlighter::selectPrevious(const QString &select)
{
	bool found = chatMessagesView()->findText(select, QWebPage::FindWrapsAroundDocument | QWebPage::FindBackward);
	chatMessagesView()->updateAtBottom();

	emit somethingFound(found);
}

void WebViewHighlighter::clearSelect()
{
	chatMessagesView()->findText(QString(), 0);
	chatMessagesView()->updateAtBottom();

	emit somethingFound(true);

	updateHighlighting();
}

#include "moc_web-view-highlighter.cpp"
