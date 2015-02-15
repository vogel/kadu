/*
 * %kadu copyright begin%
 * Copyright 2009, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2012 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2011, 2013, 2014 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2008, 2009, 2010, 2011, 2013, 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include <QtWidgets/QApplication>
#include <QtWidgets/QLabel>
#include <QtWidgets/QListWidget>

#include "gui/widgets/configuration/config-action-button.h"
#include "gui/widgets/configuration/config-group-box.h"

#include "debug.h"

ConfigActionButton::ConfigActionButton(const QString &widgetCaption, const QString &toolTip, ConfigGroupBox *parentConfigGroupBox, ConfigurationWindowDataManager *dataManager)
	: QPushButton(parentConfigGroupBox->widget()), ConfigWidget(widgetCaption, toolTip, parentConfigGroupBox, dataManager)
{
	createWidgets();
}

ConfigActionButton::ConfigActionButton(ConfigGroupBox *parentConfigGroupBox, ConfigurationWindowDataManager *dataManager)
	: QPushButton(parentConfigGroupBox->widget()), ConfigWidget(parentConfigGroupBox, dataManager)
{
}

void ConfigActionButton::createWidgets()
{
	kdebugf();

	setText(QCoreApplication::translate("@default", widgetCaption.toUtf8().constData()));
	parentConfigGroupBox->addWidgets(new QLabel(" "), this);

	if (!ConfigWidget::toolTip.isEmpty())
		setToolTip(QCoreApplication::translate("@default", ConfigWidget::toolTip.toUtf8().constData()));
}

#include "moc_config-action-button.cpp"
