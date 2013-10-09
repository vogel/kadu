/*
 * %kadu copyright begin%
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2004 Adrian Smarzewski (adrian@kadu.net)
 * Copyright 2007, 2008, 2009, 2010, 2011, 2012 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2012 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2004, 2006 Marcin Ślusarz (joi@kadu.net)
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

#ifndef HISTORY_TAB_H
#define HISTORY_TAB_H

#include <QtGui/QWidget>

/**
 * @addtogroup History
 * @{
 */

/**
 * @class HistoryTab
 * @author Rafał 'Vogel' Malinowski
 * @short Widget that provides tab content for history window.
 *
 * Each HistoryTab widget can display different view of history - like chat, status or smses history,
 * a search view or anything else. Only common think is tab layout - one widget on the left and two
 * widgets on a right separated by splitters. To maintain coherent look of all tabs, each tab must
 * implement two methods to setting and getting sizez of these splitters - size() and setSize()
 * methods.
 */
class HistoryTab : public QWidget
{
	Q_OBJECT

public:
	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Creates new HistoryTab widget.
	 * @param parent parent widget
	 */
	explicit HistoryTab(QWidget *parent = 0);
	virtual ~HistoryTab();

	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Returns sizes of subwidgets.
	 * @return sizes of subwidgets
	 *
	 * This list contains 4 values: width of left widget, width of right widget, height
	 * of top-right widget and height of bottom-right widget. It is enought to describe sizez
	 * of whole widget.
	 */
	virtual QList<int> sizes() const = 0;

	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Sets sizes of subwidgets.
	 * @param newSizes new sizes of subwidgets
	 *
	 * Passed list must contain 4 values: new width of left widget, new width of right widget,
	 * new height of top-right widget and new height of bottom-right widget.
	 * If it does not, assertion is thrown.
	 */
	virtual void setSizes(const QList<int> &newSizes) = 0;

};

/**
 * @}
 */

#endif // HISTORY_TAB_H
