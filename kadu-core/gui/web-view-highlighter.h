/*
 * %kadu copyright begin%
 * Copyright 2012 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2012, 2013, 2014 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2012 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#include "exports.h"

class WebkitMessagesView;

/**
 * @addtogroup Gui
 * @{
 */

/**
 * @class WebViewHighlighter
 * @author Rafał 'Vogel' Malinowski
 * @short This class is a highlighter for given WebkitMessagesView instance.
 *
 * This class is a highlighter for given WebkitMessagesView instance. It is able to highlight any instance
 * of given text in WebkitMessagesView as well as select any other text and go throught web view content
 * and selecting next/previous instance of it. Please note that highlighting is independend from
 * selection.
 */
class KADUAPI WebViewHighlighter : public QObject
{
	Q_OBJECT

	bool AutoUpdate;
	QString HighlightString;

	WebkitMessagesView * chatMessagesView() const;

public:
	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Creates new WebViewHighlighter on given WebkitMessagesView.
	 * @param parent WebkitMessagesView parent of new highlighter
	 *
	 * Page in main frame of parent WebkitMessagesView will have given string highlighted.
	 * Use setHighlight() to set text to highlight.
	 */
	explicit WebViewHighlighter(WebkitMessagesView *parent);
	virtual ~WebViewHighlighter();

	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Set auto update value for highlighting.
	 * @param autoUpdate new auto update value for highlighting
	 *
	 * If this property is true then any change in content of web view will trigger update of
	 * highlighting.
	 */
	void setAutoUpdate(const bool autoUpdate);

public slots:
	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Sets new text to highlight.
	 * @param highlightString new text to highlight
	 *
	 * All instances of highlightString will be highlighted in web view. If auto update is set to true
	 * after any change in content of web view highlighting will be updated to cover new content.
	 */
	void setHighlight(const QString &highlightString);

	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Updates highlighting on WebkitMessagesView.
	 *
	 * When auto udpate is set to true this method is called automatically when content size of parent
	 * WebkitMessagesView changes or when highlight string changes.
	 */
	void updateHighlighting();

	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Clears highlighting on WebkitMessagesView.
	 *
	 * Temporary removes highlighting from web view. If auto update is set to true then any change in
	 * content of web view will redo highligting. Use setHighlight(QString()) to remove highlighting
	 * for good.
	 */
	void clearHighlighting();

	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Select next found text in WebkitMessagesView.
	 * @param select text to select
	 *
	 * This method moves selection to next found instance of select string. If there is no selection,
	 * this method will move to first instance of select string.
	 */
	void selectNext(const QString &select);

	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Select previous found text in WebkitMessagesView.
	 * @param select text to select
	 *
	 * This method moves selection to previous found instance of select string. If there is no selection,
	 * this method will move to last instance of select string.
	 */
	void selectPrevious(const QString &select);

	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Clears found text in WebkitMessagesView.
	 *
	 * This method clears selection in web view.
	 */
	void clearSelect();

signals:
	void somethingFound(bool found);
};

/**
 * @}
 */

#endif // WEB_VIEW_HIGHLIGHTER_H
