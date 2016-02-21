/*
 * %kadu copyright begin%
 * Copyright 2012 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2011 Piotr Dąbrowski (ultr@ultr.pl)
 * Copyright 2011, 2012, 2013, 2014 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#include "hints-configuration-ui-handler.h"

#include "gui/widgets/configuration/configuration-widget.h"
#include "gui/windows/main-configuration-window.h"

#include <QtWidgets/QSpinBox>

HintsConfigurationUiHandler::HintsConfigurationUiHandler(QObject *parent):
		QObject{parent},
		minimumWidth{},
		maximumWidth{}
{
}

HintsConfigurationUiHandler::~HintsConfigurationUiHandler()
{
}

void HintsConfigurationUiHandler::mainConfigurationWindowCreated(MainConfigurationWindow *mainConfigurationWindow)
{
	connect(mainConfigurationWindow->widget()->widgetById("hints/showContent"), SIGNAL(toggled(bool)),
		mainConfigurationWindow->widget()->widgetById("hints/showContentCount"), SLOT(setEnabled(bool)));

	minimumWidth = static_cast<QSpinBox *>(mainConfigurationWindow->widget()->widgetById("hints/minimumWidth"));
	maximumWidth = static_cast<QSpinBox *>(mainConfigurationWindow->widget()->widgetById("hints/maximumWidth"));
	connect(minimumWidth, SIGNAL(valueChanged(int)), this, SLOT(minimumWidthChanged(int)));
	connect(maximumWidth, SIGNAL(valueChanged(int)), this, SLOT(maximumWidthChanged(int)));
}

void HintsConfigurationUiHandler::mainConfigurationWindowDestroyed()
{
}

void HintsConfigurationUiHandler::mainConfigurationWindowApplied()
{
}

void HintsConfigurationUiHandler::minimumWidthChanged(int value)
{
	if (value > maximumWidth->value())
		maximumWidth->setValue(value);
}

void HintsConfigurationUiHandler::maximumWidthChanged(int value)
{
	if (value < minimumWidth->value())
		minimumWidth->setValue(value);
}

#include "moc_hints-configuration-ui-handler.cpp"
