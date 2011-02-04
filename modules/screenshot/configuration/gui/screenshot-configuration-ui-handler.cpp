/*
 * %kadu copyright begin%
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2008, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2009 Michał Podsiadlik (michal@kadu.net)
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

#include <QtGui/QImageWriter>

#include "misc/path-conversion.h"

#include "gui/widgets/configuration/configuration-widget.h"
#include "gui/widgets/configuration/config-combo-box.h"

#include "screenshot-configuration-ui-handler.h"

ScreenShotConfigurationUiHandler *ScreenShotConfigurationUiHandler::Instance = 0;

void ScreenShotConfigurationUiHandler::registerConfigurationUi()
{
	if (Instance)
		return;

	Instance = new ScreenShotConfigurationUiHandler();

	MainConfigurationWindow::registerUiFile(dataPath("kadu/modules/configuration/screenshot.ui"));
	MainConfigurationWindow::registerUiHandler(Instance);
}

void ScreenShotConfigurationUiHandler::unregisterConfigurationUi()
{
	MainConfigurationWindow::unregisterUiFile(dataPath("kadu/modules/configuration/screenshot.ui"));

	if (Instance)
		MainConfigurationWindow::unregisterUiHandler(Instance);

	delete Instance;
	Instance = 0;
}

ScreenShotConfigurationUiHandler::ScreenShotConfigurationUiHandler(QObject *parent) :
		ConfigurationUiHandler()
{
	Q_UNUSED(parent)
}

ScreenShotConfigurationUiHandler::~ScreenShotConfigurationUiHandler()
{
}

void ScreenShotConfigurationUiHandler::mainConfigurationWindowCreated(MainConfigurationWindow *mainConfigurationWindow)
{
	connect(mainConfigurationWindow->widget()->widgetById("screenshot/enableSizeLimit"), SIGNAL(toggled(bool)),
			mainConfigurationWindow->widget()->widgetById("screenshot/sizeLimit"), SLOT(setEnabled(bool)));

	QStringList opts;
	QList<QByteArray> byteArrayOpts = QImageWriter::supportedImageFormats();

	foreach (const QByteArray &opt, byteArrayOpts)
		opts.append(QString(opt));

	ConfigComboBox *formats = static_cast<ConfigComboBox *>(mainConfigurationWindow->widget()->widgetById("screenshot/formats"));
	if (formats)
		formats->setItems(opts, opts);
}
