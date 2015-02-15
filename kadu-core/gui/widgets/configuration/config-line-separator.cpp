/*
 * %kadu copyright begin%
 * Copyright 2010 Piotr Dąbrowski (ultr@ultr.pl)
 * Copyright 2011, 2013, 2014 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2011, 2013, 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "gui/widgets/configuration/config-group-box.h"
#include "gui/widgets/configuration/config-line-separator.h"

#include "debug.h"

ConfigLineSeparator::ConfigLineSeparator(ConfigGroupBox *parentConfigGroupBox, ConfigurationWindowDataManager *dataManager)
	: QFrame(parentConfigGroupBox->widget()), ConfigWidget(parentConfigGroupBox, dataManager)
{
	createWidgets();
}

void ConfigLineSeparator::createWidgets()
{
	kdebugf();
	int margin = 5;
	setFrameShape(QFrame::HLine);
	setMinimumHeight(2*margin+1);
	parentConfigGroupBox->addWidget(this, true);
}

#include "moc_config-line-separator.cpp"
