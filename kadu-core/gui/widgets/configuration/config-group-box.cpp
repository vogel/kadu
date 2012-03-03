/*
 * %kadu copyright begin%
 * Copyright 2009, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2008, 2009, 2010, 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2010, 2011 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#include <QtGui/QGridLayout>
#include <QtGui/QGroupBox>
#include <QtGui/QLayout>
#include <QtGui/QWidget>

#include "gui/widgets/configuration/config-group-box.h"
#include "gui/widgets/configuration/config-tab.h"
#include "gui/widgets/configuration/config-widget.h"

ConfigGroupBox::ConfigGroupBox(const QString &name, ConfigTab *configTab, QGroupBox *groupBox) :
		QObject(configTab), Name(name), MyConfigTab(configTab), GroupBox(groupBox), RefCount(0)
{
	Container = new QWidget(GroupBox);
	GroupBox->layout()->addWidget(Container);

	GridLayout = new QGridLayout(Container);
	GridLayout->setContentsMargins(5, 5, 5, 5);
	GridLayout->setColumnStretch(1, 100);
}

ConfigGroupBox::~ConfigGroupBox()
{
	/* NOTE: It's needed to call ConfigTab::configGroupBoxDestroyed() before this
	 * ConfigSection will be destroyed. If we relied on QObject to send this signal,
	 * it'd be called after destroying all ConfigGroupBox data but we need that data.
	 */
	blockSignals(false);
	emit destroyed(this);

	delete GroupBox;
	GroupBox = 0;
}

void ConfigGroupBox::addWidget(QWidget *widget, bool fullSpace)
{
	int numRows = GridLayout->rowCount();

	if (fullSpace)
		GridLayout->addWidget(widget, numRows, 0, 1, 2);
	else
		GridLayout->addWidget(widget, numRows, 1);
}

void ConfigGroupBox::addWidgets(QWidget *widget1, QWidget *widget2, Qt::Alignment alignment)
{
	int numRows = GridLayout->rowCount();

	if (widget1)
		GridLayout->addWidget(widget1, numRows, 0, alignment);

	if (widget2)
		GridLayout->addWidget(widget2, numRows, 1);
}

void ConfigGroupBox::insertWidget(int pos, QWidget *widget, bool fullSpace)
{
	if (fullSpace)
		GridLayout->addWidget(widget, pos, 0, 1, 2);
	else
		GridLayout->addWidget(widget, pos, 1);
}

void ConfigGroupBox::insertWidgets(int pos, QWidget *widget1, QWidget *widget2)
{
	if (widget1)
		GridLayout->addWidget(widget1, pos, 0, Qt::AlignRight);

	if (widget2)
		GridLayout->addWidget(widget2, pos, 1);
}
