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

#include <QtGui/QApplication>
#include <QtGui/QHBoxLayout>
#include <QtGui/QKeyEvent>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QStyle>
#include <QtGui/QToolButton>

#include "search-bar.h"

SearchBar::SearchBar(QWidget *parent) :
		QToolBar(parent), SearchWidget(0)
{
	createGui();

	hide();
}

SearchBar::~SearchBar()
{
}

void SearchBar::createGui()
{
	QToolButton *closeButton = new QToolButton(this);
	closeButton->setIcon(qApp->style()->standardIcon(QStyle::SP_DialogCloseButton));
	connect(closeButton, SIGNAL(clicked()), this, SLOT(hide()));
	addWidget(closeButton);

	addWidget(new QLabel(tr("Find:"), this));

	FindEdit = new QLineEdit(this);
	addWidget(FindEdit);

	QToolButton *previousButton = new QToolButton(this);
	previousButton->setIcon(qApp->style()->standardIcon(QStyle::SP_ArrowLeft));
	previousButton->setText(tr("Previous"));
	connect(previousButton, SIGNAL(clicked(bool)), this, SLOT(previous()));
	addWidget(previousButton);

	QToolButton *nextButton = new QToolButton(this);
	nextButton->setIcon(qApp->style()->standardIcon(QStyle::SP_ArrowRight));
	nextButton->setText(tr("Next"));
	connect(nextButton, SIGNAL(clicked(bool)), this, SLOT(next()));
	addWidget(nextButton);
}

void SearchBar::keyPressEvent(QKeyEvent *event)
{
	switch (event->key())
	{
		case Qt::Key_Escape:
		{
			event->accept();
			emit clearSearch();
			hide();
			if (SearchWidget)
				SearchWidget->setFocus();
			break;
		}

		case Qt::Key_Enter:
		case Qt::Key_Return:
		{
			if (Qt::ShiftModifier == event->modifiers())
				previous();
			else
				next();
			break;
		}

		default:
			QWidget::keyPressEvent(event);
	}
}

void SearchBar::showEvent(QShowEvent *event)
{
	FindEdit->setText(QString());
	QWidget::showEvent(event);
}

void SearchBar::setSearchWidget(QWidget * const widget)
{
	if (SearchWidget)
	{
		SearchWidget->removeEventFilter(this);
		disconnect(SearchWidget, SIGNAL(destroyed()), this, SLOT(searchWidgetDestroyed()));
	}

	SearchWidget = widget;

	if (SearchWidget)
	{
		SearchWidget->installEventFilter(this);
		connect(SearchWidget, SIGNAL(destroyed()), this, SLOT(searchWidgetDestroyed()));
	}
}

void SearchBar::searchWidgetDestroyed()
{
	SearchWidget = 0;
}

bool SearchBar::eventFilter(QObject *object, QEvent *event)
{
	Q_ASSERT(object == SearchWidget);

	if (QEvent::KeyPress != event->type())
		return false;

	QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);
	if (Qt::Key_F == keyEvent->key() && Qt::ControlModifier == keyEvent->modifiers())
	{
		show();
		FindEdit->setFocus();
		return true;
	}

	return false;
}

void SearchBar::setSearchText(const QString &search)
{
	FindEdit->setText(search);
}

void SearchBar::previous()
{
	if (!FindEdit->text().isEmpty())
		emit searchPrevious(FindEdit->text());
}

void SearchBar::next()
{
	if (!FindEdit->text().isEmpty())
		emit searchNext(FindEdit->text());
}
