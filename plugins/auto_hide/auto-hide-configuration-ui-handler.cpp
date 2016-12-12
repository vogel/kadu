/*
 * %kadu copyright begin%
 * Copyright 2015 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "auto-hide-configuration-ui-handler.h"

#include "widgets/configuration/configuration-widget.h"
#include "windows/main-configuration-window.h"

#include <QtWidgets/QSpinBox>

AutoHideConfigurationUiHandler::AutoHideConfigurationUiHandler(QObject *parent) :
		QObject{parent}
{
}

AutoHideConfigurationUiHandler::~AutoHideConfigurationUiHandler()
{
}

void AutoHideConfigurationUiHandler::mainConfigurationWindowCreated(MainConfigurationWindow *mainConfigurationWindow)
{
	QObject::connect(
			mainConfigurationWindow->widget()->widgetById("auto_hide/use_auto_hide"), SIGNAL(toggled(bool)),
			mainConfigurationWindow->widget()->widgetById("auto_hide/idle_time"), SLOT(setEnabled(bool)));
	static_cast<QSpinBox *>(mainConfigurationWindow->widget()->widgetById("auto_hide/idle_time"))->setSpecialValueText(tr("Don't hide"));
}

void AutoHideConfigurationUiHandler::mainConfigurationWindowDestroyed()
{
}

void AutoHideConfigurationUiHandler::mainConfigurationWindowApplied()
{
}

#include "moc_auto-hide-configuration-ui-handler.cpp"
