/*
 * %kadu copyright begin%
 * Copyright 2012 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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
#include <QtWebKit/QWebView>

#include "web-view-highlighter.h"

WebViewHighlighter::WebViewHighlighter(QWebView *parent) :
		QObject(parent), AutoUpdate(false)
{
}

WebViewHighlighter::~WebViewHighlighter()
{
}

QWebView * WebViewHighlighter::webView() const
{
	return static_cast<QWebView *>(parent());
}

void WebViewHighlighter::setAutoUpdate(const bool autoUpdate)
{
	if (AutoUpdate == autoUpdate)
		return;

	// we might assume that when any message is added to web view its size changes
	// unfortunately contentChanged() does not work when content is updated by javascript
	// inside webkit instance

	if (AutoUpdate)
		disconnect(webView()->page()->mainFrame(), SIGNAL(contentsSizeChanged(QSize)),
		           this, SLOT(updateHighlighting()));

	AutoUpdate = autoUpdate;

	if (AutoUpdate)
		connect(webView()->page()->mainFrame(), SIGNAL(contentsSizeChanged(QSize)),
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

	// reset to first occurence
	webView()->findText(QString(), QWebPage::FindWrapsAroundDocument);
	webView()->findText(HighlightString, QWebPage::FindWrapsAroundDocument);

	// highlight all other
	webView()->findText(HighlightString, QWebPage::HighlightAllOccurrences);
}

void WebViewHighlighter::clearHighlighting()
{
	webView()->findText(QString(), QWebPage::HighlightAllOccurrences);
}

void WebViewHighlighter::selectNext(const QString &select)
{
	webView()->findText(select, QWebPage::FindWrapsAroundDocument);
}

void WebViewHighlighter::selectPrevious(const QString &select)
{
	webView()->findText(select, QWebPage::FindWrapsAroundDocument | QWebPage::FindBackward);
}

void WebViewHighlighter::clearSelect()
{
	webView()->findText(QString(), 0);

	updateHighlighting();
}
