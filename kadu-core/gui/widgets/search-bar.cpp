/*
 * %kadu copyright begin%
 * Copyright 2012 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2012, 2013, 2014 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2012, 2013, 2014 Bartosz Brachaczek (b.brachaczek@gmail.com)
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
		QToolBar(parent), AutoVisibility(true)
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
	closeButton->setFixedSize(QSize(16, 16));
	connect(closeButton, SIGNAL(clicked()), this, SLOT(hide()));
	addWidget(closeButton);

	addWidget(new QLabel(tr("Find:"), this));

	FindEdit = new QLineEdit(this);
	connect(FindEdit, SIGNAL(textChanged(QString)), this, SLOT(searchTextChanged(QString)));
	addWidget(FindEdit);

	QToolButton *previousButton = new QToolButton(this);
	previousButton->setIcon(qApp->style()->standardIcon(QStyle::SP_ArrowLeft));
	previousButton->setText(tr("Previous"));
	previousButton->setFixedSize(QSize(16, 16));
	connect(previousButton, SIGNAL(clicked(bool)), this, SLOT(previous()));
	addWidget(previousButton);

	QToolButton *nextButton = new QToolButton(this);
	nextButton->setIcon(qApp->style()->standardIcon(QStyle::SP_ArrowRight));
	nextButton->setFixedSize(QSize(16, 16));
	nextButton->setText(tr("Next"));
	connect(nextButton, SIGNAL(clicked(bool)), this, SLOT(next()));
	addWidget(nextButton);
}

void SearchBar::keyPressEvent(QKeyEvent *event)
{
	if (Qt::Key_F == event->key() && Qt::ControlModifier == event->modifiers())
	{
		event->accept();
		close();
		return;
	}

	switch (event->key())
	{
		case Qt::Key_Escape:
		{
			event->accept();
			close();
			break;
		}

		case Qt::Key_Enter:
		case Qt::Key_Return:
		case Qt::Key_F3:
		{
			if (Qt::ShiftModifier == event->modifiers())
				previous();
			else
				next();
			break;
		}

		default:
		{
			somethingFound(true);

			QWidget::keyPressEvent(event);
		}
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
		SearchWidget->removeEventFilter(this);

	SearchWidget = widget;

	if (SearchWidget)
		SearchWidget->installEventFilter(this);
}

void SearchBar::setAutoVisibility(bool autoVisibility)
{
	if (AutoVisibility == autoVisibility)
		return;

	AutoVisibility = autoVisibility;
	if (!AutoVisibility)
		show();
}

bool SearchBar::eventFilter(QObject *object, QEvent *event)
{
	if (QEvent::KeyPress != event->type() || object != SearchWidget.data())
		return false;

	QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);
	if ((Qt::Key_F == keyEvent->key() && Qt::ControlModifier == keyEvent->modifiers()) ||
	    (Qt::Key_F3 == keyEvent->key()))
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

void SearchBar::close()
{
	FindEdit->setText(QString());
	emit clearSearch();

	if (AutoVisibility)
		hide();

	if (SearchWidget)
		SearchWidget->setFocus();
}

void SearchBar::searchTextChanged(const QString &text)
{
	Q_UNUSED(text)

	somethingFound(true);
}

void SearchBar::somethingFound(bool found)
{
	QString style = "";

	if (!found)
		style = "QLineEdit{background: #FFB4B4;}";

	FindEdit->setStyleSheet(style);
}

#include "moc_search-bar.cpp"
