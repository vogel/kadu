/*
 * %kadu copyright begin%
 * Copyright 2012, 2014 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2011, 2012, 2013, 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#ifndef FILTERED_TREE_VIEW_H
#define FILTERED_TREE_VIEW_H

#include <QtWidgets/QWidget>

#include "exports.h"

class QAbstractItemView;
class QVBoxLayout;

class FilterWidget;

class KADUAPI FilteredTreeView : public QWidget
{
	Q_OBJECT

public:
	enum FilterPosition
	{
		FilterAtTop,
		FilterAtBottom
	};

private:
	FilterPosition CurrentFilterPosition;

	QVBoxLayout *Layout;
	FilterWidget *NameFilterWidget;
	QAbstractItemView *View;

	void removeView();
	void insertView();

protected:
	virtual void keyPressEvent(QKeyEvent *event);

public:
	static bool shouldEventGoToFilter(QKeyEvent *event);

	explicit FilteredTreeView(FilterPosition filterPosition, QWidget *parent = 0, Qt::WindowFlags f = 0);
	virtual ~FilteredTreeView();

	FilterWidget * filterWidget() { return NameFilterWidget; }

	void setPosition(FilterPosition filterPosition);
	void setView(QAbstractItemView *view);

signals:
	void filterChanged(const QString &filterText);

};

#endif // FILTERED_TREE_VIEW_H
