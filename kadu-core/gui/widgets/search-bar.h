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

#ifndef SEARCH_BAR_H
#define SEARCH_BAR_H

#include <QtGui/QToolBar>

class QEvent;
class QLineEdit;

/**
 * @addtogroup Gui
 * @{
 */

/**
 * @class SearchBar
 * @author Rafał 'Vogel' Malinowski
 * @short Firefox-like search bar for web views.
 *
 * This widget is Firefox-like search bar for web views. It has 3 buttons: Close, Previous and Next as well as Find line edit.
 * Signal clearSearch() is emited when this widget is hidden by Esc key or clicking Close button. Signals searchPrevious() and
 * searchNext() are emited when proper button is clicked and search string is not empty.
 *
 * Text in line edit can be set by setSearchText() method. If search widget is set by setSearchWidget() method then this search
 * bar will intercept any Ctrl+F keypesses from it and show itself.
 */
class SearchBar : public QToolBar
{
	Q_OBJECT

	QWidget *SearchWidget;

	QLineEdit *FindEdit;

	void createGui();

private slots:
	void searchWidgetDestroyed();

	void previous();
	void next();

protected:
	virtual bool eventFilter(QObject *object, QEvent *event);
	virtual void keyPressEvent(QKeyEvent *event);
	virtual void showEvent(QShowEvent *event);

public:
	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Creates new empty SearchBar.
	 *
	 * SearchBar is hidden by default. Use show() to make it visible or set up search widget to allow use show it by using Ctrl+F
	 * shortcut.
	 */
	explicit SearchBar(QWidget *parent = 0);
	virtual ~SearchBar();

	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Set search widget for this search bar.
	 * @param widget new search widget for this search bar
	 *
	 * SearchBar will attach itself to keypress event of widget. When Ctrl+F shortcut is used on this widget then search bar
	 * widget will show itself.
	 */
	void setSearchWidget(QWidget * const widget);

public slots:
	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Set text visible in find line edit.
	 * @param search text visible in find line edit
	 *
	 * Calling this method will not emit any signal.
	 */
	void setSearchText(const QString &search);

signals:
	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Signal emited when previous button is clicked and search string is not empty.
	 * @param search search string entered in Find line edit.
	 */
	void searchPrevious(const QString &search);

	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Signal emited when next button is clicked and search string is not empty.
	 * @param search search string entered in Find line edit.
	 */
	void searchNext(const QString &search);

	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Signal emited when widget is being closed.
	 *
	 * Widget can be closed by Close button or by Esc key.
	 */
	void clearSearch();

};

/**
 * @}
 */

#endif // SEARCH_BAR_H
