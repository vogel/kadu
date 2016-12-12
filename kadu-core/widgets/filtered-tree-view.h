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

#include <QtCore/QPointer>
#include <QtWidgets/QWidget>
#include <injeqt/injeqt.h>

#include "exports.h"

class QAbstractItemView;
class QVBoxLayout;

class FilterWidget;
class InjectedFactory;

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
	QPointer<InjectedFactory> m_injectedFactory;

	FilterPosition CurrentFilterPosition;

	QVBoxLayout *Layout;
	FilterWidget *NameFilterWidget;
	QAbstractItemView *View;

	void removeView();
	void insertView();

private slots:
	INJEQT_SET void setInjectedFactory(InjectedFactory *injectedFactory);
	INJEQT_INIT void init();

protected:
	InjectedFactory * injectedFactory() const;

	virtual void keyPressEvent(QKeyEvent *event);

public:
	static bool shouldEventGoToFilter(QKeyEvent *event);

	explicit FilteredTreeView(FilterPosition filterPosition, QWidget *parent = nullptr, Qt::WindowFlags f = 0);
	virtual ~FilteredTreeView();

	FilterWidget * filterWidget() { return NameFilterWidget; }

	void setPosition(FilterPosition filterPosition);
	void setView(QAbstractItemView *view);

signals:
	void filterChanged(const QString &filterText);

};

#endif // FILTERED_TREE_VIEW_H
