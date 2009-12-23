/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <QtCore/QEvent>
#include <QtGui/QApplication>
#include <QtGui/QHBoxLayout>
#include <QtGui/QKeyEvent>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QVBoxLayout>

#include "buddies/filter/buddy-name-filter.h"
#include "gui/widgets/buddies-list-view.h"

#include "buddies-list-widget.h"

BuddiesListWidget::BuddiesListWidget(FilterPosition filterPosition, MainWindow *mainWindow, QWidget *parent) :
		QWidget(parent)
{
	QVBoxLayout *layout = new QVBoxLayout(this);
	layout->setMargin(0);

	QWidget *topWidget = new QWidget(this);

	QHBoxLayout *topLayout = new QHBoxLayout(topWidget);
	topLayout->addWidget(new QLabel(tr("Filter") + ":", this));
	topLayout->setMargin(0);

	NameFilterEdit = new QLineEdit(this);
	NameFilterEdit->installEventFilter(this);

	topLayout->addWidget(NameFilterEdit);
	connect(NameFilterEdit, SIGNAL(textChanged(const QString &)),
			this, SLOT(nameFilterChanged(const QString &)));
	connect(NameFilterEdit, SIGNAL(next()), this, SLOT(selectNext()));
	connect(NameFilterEdit, SIGNAL(previous()), this, SLOT(selectPrevious()));

	View = new BuddiesListView(mainWindow, this);

	NameFilter = new BuddyNameFilter(this);
	View->addFilter(NameFilter);

	if (FilterAtTop == filterPosition)
	{
		layout->addWidget(topWidget);
		layout->addWidget(View);
	}
	else
	{
		layout->addWidget(View);
		layout->addWidget(topWidget);
	}

/*
	setFocusProxy(NameFilterEdit);
	View->setFocusPolicy(Qt::NoFocus);*/
}

BuddiesListWidget::~BuddiesListWidget()
{
}


bool BuddiesListWidget::eventFilter(QObject *object, QEvent *event)
{
	if (QEvent::KeyPress == event->type())
	{
		QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);
		switch (keyEvent->key())
		{
			case Qt::Key_Down:
			case Qt::Key_Up:
			case Qt::Key_PageDown:
			case Qt::Key_PageUp:
			case Qt::Key_Enter:
			case Qt::Key_Return:
				qApp->sendEvent(view(), event);
				return true;
		}
	}

	return QObject::eventFilter(object, event);
}


void BuddiesListWidget::selectNext()
{/*
	QModelIndexList selection = View->selectionModel()->selectedIndexes();
	QModelIndex index;

	if (0 == selection.size())
		index = View->model()->index(0, 0);
	else
	{
		QModelIndex first = selection.first();
		QModelIndex next = View->indexBelow(first);
		selection.clear();
		if (next.isValid())
			index = next;
		else
			index = first;
	}

	View->selectionModel()->setCurrentIndex(index, QItemSelectionModel::SelectCurrent);*/
}

void BuddiesListWidget::selectPrevious()
{/*
	QModelIndexList selection = View->selectionModel()->selectedIndexes();
	QModelIndex index;
	
	if (0 == selection.size())
		index = View->model()->index(View->model()->rowCount() - 1, 0);
	else
	{
		QModelIndex first = selection.first();
		QModelIndex previous = View->indexAbove(first);
		selection.clear();
		if (previous.isValid())
			index = previous;
		else
			index = first;
	}
	
	View->selectionModel()->setCurrentIndex(index, QItemSelectionModel::SelectCurrent);*/
}

void BuddiesListWidget::nameFilterChanged(const QString &filter)
{
	NameFilter->setName(filter);
}
