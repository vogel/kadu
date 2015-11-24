/*
 * %kadu copyright begin%
 * Copyright 2010, 2012 Tomasz Rostanski (rozteck@interia.pl)
 * Copyright 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2012 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2010 Tomasz Rostański (rozteck@interia.pl)
 * Copyright 2012 Piotr Dąbrowski (ultr@ultr.pl)
 * Copyright 2010, 2011, 2013, 2014 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2010, 2011, 2012, 2013, 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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
 *
 * Mac OS X implementation based on searchbox class by Matteo Bertozzi:
 * http://th30z.netsons.org/2008/08/qt4-mac-searchbox-wrapper/
 */

#include <QtCore/QCoreApplication>
#include <QtGui/QKeyEvent>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>

#include "gui/widgets/line-edit-with-clear-button.h"
#include "gui/widgets/talkable-tree-view.h"

#include "filter-widget.h"


QString FilterWidget::filterText() const
{
	return NameFilterEdit->text();
}

void FilterWidget::updateVisibility()
{
	if (!AutoVisibility)
	{
		show();
		return;
	}

	if (NameFilterEdit->text().isEmpty())
		hide();
	else
		show();
}

void FilterWidget::filterTextChanged(const QString &s)
{
	emit textChanged(s);

	if (!View)
		return;

	if (NameFilterEdit->text().isEmpty())
	{
		QModelIndexList selection = View->selectionModel()->selectedIndexes();
		if (!selection.isEmpty())
		{
			qSort(selection);
			View->scrollTo(selection.at(0));
		}
	}
	else
	{
		if (!isVisible() || View->selectionModel()->selectedIndexes().isEmpty())
		{
			View->setCurrentIndex(View->model()->index(0, 0));
			View->selectionModel()->select(View->model()->index(0, 0), QItemSelectionModel::SelectCurrent);
		}
	}

	updateVisibility();
}

FilterWidget::FilterWidget(QWidget *parent) :
		QWidget(parent), View(0), AutoVisibility(true)
{
	QHBoxLayout *layout = new QHBoxLayout(this);
	layout->setMargin(3);

	NameFilterEdit = new LineEditWithClearButton(this);
	Label = new QLabel(tr("Search") + ":", this);

	setFocusProxy(NameFilterEdit);

	layout->addWidget(Label);
	layout->addWidget(NameFilterEdit);

	connect(NameFilterEdit, SIGNAL(textChanged(const QString &)),
			this, SLOT(filterTextChanged(const QString &)));

	updateVisibility();
}

FilterWidget::~FilterWidget()
{
}

void FilterWidget::setLabel(const QString &label)
{
	Label->setText(label);
}

void FilterWidget::setFilter(const QString &filter)
{
	NameFilterEdit->setText(filter);
}

void FilterWidget::setView(QAbstractItemView *view)
{
	View = view;
}

void FilterWidget::setAutoVisibility(bool autoVisiblity)
{
	AutoVisibility = autoVisiblity;
	updateVisibility();
}

bool FilterWidget::sendKeyEventToView(QKeyEvent *event)
{
	switch (event->key())
	{
		case Qt::Key_Enter:
		case Qt::Key_Return:
		case Qt::Key_Down:
		case Qt::Key_Up:
		case Qt::Key_PageDown:
		case Qt::Key_PageUp:
			QCoreApplication::sendEvent(View, event);
			return true;
	}

	return false;
}

void FilterWidget::keyPressEvent(QKeyEvent *event)
{
	if (event->key() == Qt::Key_Escape &&
			!NameFilterEdit->text().isEmpty()
			)
	{
		setFilter(QString());
		event->accept();
		return;
	}

	if (View && sendKeyEventToView(event))
		return;

	QWidget::keyPressEvent(event);
}

#include "moc_filter-widget.cpp"
