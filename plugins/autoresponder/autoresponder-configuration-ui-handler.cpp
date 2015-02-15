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

#include <QtWidgets/QApplication>
#include <QtWidgets/QLineEdit>

#include "core/application.h"
#include "gui/widgets/configuration/configuration-widget.h"
#include "misc/paths-provider.h"

#include "autoresponder-configuration-ui-handler.h"

AutoresponderConfigurationUiHolder::AutoresponderConfigurationUiHolder(QObject *parent) :
		ConfigurationUiHandler(parent)
{
	MainConfigurationWindow::registerUiFile(Application::instance()->pathsProvider()->dataPath() + QLatin1String("plugins/configuration/autoresponder.ui"));
}

AutoresponderConfigurationUiHolder::~AutoresponderConfigurationUiHolder()
{
	MainConfigurationWindow::unregisterUiFile(Application::instance()->pathsProvider()->dataPath() + QLatin1String("plugins/configuration/autoresponder.ui"));
}

void AutoresponderConfigurationUiHolder::mainConfigurationWindowCreated(MainConfigurationWindow *mainConfigurationWindow)
{
	QLineEdit *autoRespondTextLineEdit = qobject_cast<QLineEdit *>(mainConfigurationWindow->widget()->widgetById("autoresponder/autoRespondText"));
	autoRespondTextLineEdit->setToolTip(QCoreApplication::translate("@default", MainConfigurationWindow::SyntaxText));
}

#include "moc_autoresponder-configuration-ui-handler.cpp"
