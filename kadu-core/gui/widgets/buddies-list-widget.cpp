/*
 * %kadu copyright begin%
 * Copyright 2010 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2009, 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009, 2010 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2010 Przemysław Rudy (prudy1@o2.pl)
 * Copyright 2009, 2010, 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2010 Tomasz Rostański (rozteck@interia.pl)
 * Copyright 2009 Longer (longer89@gmail.com)
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

#include <QtGui/QHBoxLayout>
#include <QtGui/QKeyEvent>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QVBoxLayout>

#include "buddies/filter/anonymous-without-messages-buddy-filter.h"
#include "buddies/filter/buddy-name-filter.h"
#include "buddies/filter/pending-messages-filter.h"
#include "gui/widgets/buddies-list-view.h"
#include "icons/icons-manager.h"

#include "buddies-list-widget.h"

BuddiesListWidget::BuddiesListWidget(FilterPosition filterPosition, QWidget *parent) :
		QWidget(parent)
{
	QVBoxLayout *layout = new QVBoxLayout(this);
	layout->setMargin(0);
	layout->setSpacing(0);

	NameFilterWidget = new FilterWidget(this);

	connect(NameFilterWidget, SIGNAL(textChanged(const QString &)),
		this, SLOT(nameFilterChanged(const QString &)));

	View = new BuddiesListView(this);
	View->addFilter(new PendingMessagesFilter(this));

	AnonymousFilter = new AnonymousWithoutMessagesBuddyFilter(this);
	AnonymousFilter->setEnabled(true);
	View->addFilter(AnonymousFilter);

	NameFilterWidget->setView(View);
#ifndef Q_OS_MAC
	NameFilterWidget->hide(); // hide by default
#endif
	NameFilter = new BuddyNameFilter(this);
	View->addFilter(NameFilter);

	if (FilterAtTop == filterPosition)
	{
		layout->addWidget(NameFilterWidget);
		layout->addWidget(View);
	}
	else
	{
		layout->addWidget(View);
		layout->addWidget(NameFilterWidget);
	}
}

BuddiesListWidget::~BuddiesListWidget()
{
}

void BuddiesListWidget::clearFilter()
{
	NameFilterWidget->setFilter(QString());
}

void BuddiesListWidget::setShowAnonymous(bool show)
{
	AnonymousFilter->setEnabled(!show);
}

void BuddiesListWidget::nameFilterChanged(const QString &filter)
{
	NameFilter->setName(filter);
}

void BuddiesListWidget::keyPressEvent(QKeyEvent *event)
{
	if (BuddiesListView::shouldEventGoToFilter(event))
	{
		NameFilterWidget->setFilter(event->text());
		NameFilterWidget->setFocus(Qt::OtherFocusReason);
		event->accept();
		return;
	}

	QWidget::keyPressEvent(event);
}
