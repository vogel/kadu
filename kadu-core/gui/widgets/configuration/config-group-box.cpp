/*
 * %kadu copyright begin%
 * Copyright 2009, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2012 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2010, 2011, 2013, 2014 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2008, 2009, 2010, 2011, 2012, 2013, 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include <QtWidgets/QFormLayout>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QLayout>
#include <QtWidgets/QWidget>

#include "gui/widgets/configuration/config-group-box.h"
#include "gui/widgets/configuration/config-tab.h"
#include "gui/widgets/configuration/config-widget.h"

ConfigGroupBox::ConfigGroupBox(const QString &name, ConfigTab *configTab, QGroupBox *groupBox) :
		QObject(configTab), Name(name), GroupBox(groupBox), RefCount(0)
{
	Container = new QWidget(GroupBox);
	GroupBox->layout()->addWidget(Container);

	MainLayout = new QFormLayout(Container);
	MainLayout->setContentsMargins(5, 5, 5, 5);
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
	Q_UNUSED(fullSpace);

	MainLayout->addRow(widget);
}

void ConfigGroupBox::addWidgets(QWidget *widget1, QWidget *widget2, Qt::Alignment alignment)
{
	Q_UNUSED(alignment);

	MainLayout->addRow(widget1, widget2);
}

void ConfigGroupBox::insertWidget(int pos, QWidget *widget, bool fullSpace)
{
	Q_UNUSED(fullSpace);

	MainLayout->insertRow(pos, widget);
}

void ConfigGroupBox::insertWidgets(int pos, QWidget *widget1, QWidget *widget2)
{
	MainLayout->insertRow(pos, widget1, widget2);
}

#include "moc_config-group-box.cpp"
