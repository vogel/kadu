/*
 * %kadu copyright begin%
 * Copyright 2011, 2012, 2013, 2014 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#include <QtGui/QKeyEvent>
#include <QtWidgets/QTreeView>
#include <QtWidgets/QVBoxLayout>

#include "gui/widgets/filter-widget.h"

#include "filtered-tree-view.h"

bool FilteredTreeView::shouldEventGoToFilter(QKeyEvent *event)
{
	return !event->text().trimmed().isEmpty() && event->text().at(0).isPrint();
}

FilteredTreeView::FilteredTreeView(FilteredTreeView::FilterPosition filterPosition, QWidget *parent, Qt::WindowFlags f) :
		QWidget(parent, f), CurrentFilterPosition(filterPosition), View(0)
{
	Layout = new QVBoxLayout(this);
	Layout->setMargin(0);
	Layout->setSpacing(0);

	NameFilterWidget = new FilterWidget(this);
	connect(NameFilterWidget, SIGNAL(textChanged(const QString &)),
		this, SIGNAL(filterChanged(const QString &)));

	Layout->addWidget(NameFilterWidget);
}

FilteredTreeView::~FilteredTreeView()
{
}

void FilteredTreeView::removeView()
{
	if (View)
		Layout->removeWidget(View);
}

void FilteredTreeView::insertView()
{
	if (!View)
		return;

	if (FilterAtTop == CurrentFilterPosition)
		Layout->insertWidget(1, View);
	else
		Layout->insertWidget(0, View);

	setFocusProxy(View);
}

void FilteredTreeView::setPosition(FilterPosition filterPosition)
{
	if (CurrentFilterPosition == filterPosition)
		return;

	removeView();
	CurrentFilterPosition = filterPosition;
	insertView();
}

void FilteredTreeView::setView(QAbstractItemView *view)
{
	removeView();
	View = view;
	NameFilterWidget->setView(View);
	insertView();
}

void FilteredTreeView::keyPressEvent(QKeyEvent *event)
{
	if (shouldEventGoToFilter(event))
	{
		NameFilterWidget->setFilter(event->text());
		NameFilterWidget->setFocus(Qt::OtherFocusReason);
		event->accept();
		return;
	}

	QWidget::keyPressEvent(event);
}

#include "moc_filtered-tree-view.cpp"
