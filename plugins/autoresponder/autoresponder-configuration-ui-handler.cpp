/*
 * %kadu copyright begin%
 * Copyright 2012 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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
#include <QtGui/QLineEdit>

#include "gui/widgets/configuration/configuration-widget.h"
#include "misc/kadu-paths.h"

#include "autoresponder-configuration-ui-handler.h"

AutoresponderConfigurationUiHolder::AutoresponderConfigurationUiHolder(QObject *parent) :
		ConfigurationUiHandler(parent)
{
	MainConfigurationWindow::registerUiFile(KaduPaths::instance()->dataPath() + QLatin1String("plugins/configuration/autoresponder.ui"));
}

AutoresponderConfigurationUiHolder::~AutoresponderConfigurationUiHolder()
{
	MainConfigurationWindow::unregisterUiFile(KaduPaths::instance()->dataPath() + QLatin1String("plugins/configuration/autoresponder.ui"));
}

void AutoresponderConfigurationUiHolder::mainConfigurationWindowCreated(MainConfigurationWindow *mainConfigurationWindow)
{
	QLineEdit *autoRespondTextLineEdit = qobject_cast<QLineEdit *>(mainConfigurationWindow->widget()->widgetById("autoresponder/autoRespondText"));
	autoRespondTextLineEdit->setToolTip(qApp->translate("@default", MainConfigurationWindow::SyntaxText));
}

#include "moc_autoresponder-configuration-ui-handler.cpp"
