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

#ifndef WEB_VIEW_HIGHLIGHTER_H
#define WEB_VIEW_HIGHLIGHTER_H

#include <QtCore/QObject>

class QWebView;

/**
 * @addtogroup Gui
 * @{
 */

/**
 * @class WebViewHighlighter
 * @author Rafał 'Vogel' Malinowski
 * @short This class is a highlighter for given QWebView instance.
 *
 * This class is a highlighter for given QWebView instance. Any instance of provided text
 * in QWebView will be highlighted.
 */
class WebViewHighlighter : public QObject
{
	Q_OBJECT

	QString HighlightString;

public:
	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Creates new WebViewHighlighter on given QWebView.
	 * @param parent QWebView parent of new highlighter
	 *
	 * Page in main frame of parent QWebView will have given string highlighted.
	 * Use setHighlight() to set text to highlight.
	 */
	explicit WebViewHighlighter(QWebView *parent);
	virtual ~WebViewHighlighter();

	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Sets new text to highlight.
	 * @param highlightString new text to highlight
	 */
	void setHighlight(const QString &highlightString);

public slots:
	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Updates highlighting on QWebView.
	 *
	 * This method is called automatically when content size of parent QWebView changes or
	 * when highlight string changes.
	 */
	void updateHighlighting();

	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Highlight next found text in QWebView.
	 */
	void highlightNext();

	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Highlight previous found text in QWebView.
	 */
	void highlightPrevious();

};

/**
 * @}
 */

#endif // WEB_VIEW_HIGHLIGHTER_H
