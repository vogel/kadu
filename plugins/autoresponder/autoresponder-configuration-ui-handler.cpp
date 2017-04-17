/*
 * %kadu copyright begin%
 * Copyright 2013, 2014 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2012, 2013, 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "autoresponder-configuration-ui-handler.h"
#include "autoresponder-configuration-ui-handler.moc"

#include "widgets/configuration/configuration-widget.h"
#include "windows/main-configuration-window.h"

#include <QtWidgets/QApplication>
#include <QtWidgets/QLineEdit>

AutoresponderConfigurationUiHandler::AutoresponderConfigurationUiHandler(QObject *parent) : QObject{parent}
{
}

AutoresponderConfigurationUiHandler::~AutoresponderConfigurationUiHandler()
{
}

void AutoresponderConfigurationUiHandler::mainConfigurationWindowCreated(
    MainConfigurationWindow *mainConfigurationWindow)
{
    auto autoRespondTextLineEdit =
        qobject_cast<QLineEdit *>(mainConfigurationWindow->widget()->widgetById("autoresponder/autoRespondText"));
    autoRespondTextLineEdit->setToolTip(QCoreApplication::translate("@default", MainConfigurationWindow::SyntaxText));
}

void AutoresponderConfigurationUiHandler::mainConfigurationWindowDestroyed()
{
}

void AutoresponderConfigurationUiHandler::mainConfigurationWindowApplied()
{
}
