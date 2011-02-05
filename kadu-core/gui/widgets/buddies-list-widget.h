/*
 * %kadu copyright begin%
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2009 Piotr Galiszewski (piotrgaliszewski@gmail.com)
 * Copyright 2009 Bartłomiej Zimoń (uzi18@o2.pl)
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

#ifndef BUDDIES_LIST_WIDGET_H
#define BUDDIES_LIST_WIDGET_H

#include <QtGui/QWidget>

#include "gui/widgets/filter-widget.h"
#include "exports.h"

class BuddyNameFilter;
class BuddiesListView;
class MainWindow;

class KADUAPI BuddiesListWidget : public QWidget
{
	Q_OBJECT

public:
	enum FilterPosition
	{
		FilterAtTop,
		FilterAtBottom
	};

private:
	FilterWidget *NameFilterWidget;

	BuddiesListView *View;
	BuddyNameFilter *NameFilter;

private slots:
	void nameFilterChanged(const QString &filter);

protected:
	virtual void keyPressEvent(QKeyEvent *event);

public:
	explicit BuddiesListWidget(FilterPosition filterPosition, QWidget *parent = 0);
	virtual ~BuddiesListWidget();

	void clearFilter();

	BuddiesListView * view() { return View; }
	QWidget * nameFilterWidget() { return NameFilterWidget; }
	BuddyNameFilter * nameFilter() { return NameFilter; }

};

#endif //BUDDIES_LIST_WIDGET_H
