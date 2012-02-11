/*
 * %kadu copyright begin%
 * Copyright 2009, 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2008 Tomasz Rostański (rozteck@interia.pl)
 * Copyright 2009 Maciej Płaza (plaza.maciej@gmail.com)
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

#include <QtGui/QApplication>
#include <QtGui/QHBoxLayout>
#include <QtGui/QPushButton>

#include "configuration/notifier-configuration-data-manager.h"
#include "gui/widgets/configuration/config-combo-box.h"
#include "gui/widgets/configuration/configuration-widget.h"
#include "gui/widgets/configuration/notify-group-box.h"
#include "gui/windows/configuration-window.h"
#include "gui/windows/main-configuration-window.h"
#include "misc/kadu-paths.h"

#include "activate.h"
#include "debug.h"

#include "qt4-docking-notify-configuration-widget.h"

Qt4NotifyConfigurationWidget::Qt4NotifyConfigurationWidget(QWidget *parent)
	: NotifierConfigurationWidget(parent)
{
	QPushButton *configureButton = new QPushButton(tr("Configure"));
	connect(configureButton, SIGNAL(clicked()), this, SLOT(showConfigurationWindow()));

	QHBoxLayout *layout = new QHBoxLayout(this);
	layout->addWidget(configureButton);

	static_cast<NotifyGroupBox *>(parent)->addWidget(this);
}

void Qt4NotifyConfigurationWidget::saveNotifyConfigurations()
{
}

void Qt4NotifyConfigurationWidget::switchToEvent(const QString &event)
{
	kdebugf();

	currentNotifyEvent = event;
}

void Qt4NotifyConfigurationWidget::showConfigurationWindow()
{
	NotifierConfigurationDataManager *dataManager = NotifierConfigurationDataManager::dataManagerForEvent(currentNotifyEvent);
	ConfigurationWindow *configWindow = new ConfigurationWindow("Qt4DockingNotifyEventConfiguration", tr("Tray icon balloon's look configuration"), "Qt4DockingNotify", dataManager);

	dataManager->configurationWindowCreated(configWindow);

	configWindow->widget()->appendUiFile(KaduPaths::instance()->dataPath() + QLatin1String("plugins/configuration/qt4-docking-notify.ui"));

	QString tooltip = qApp->translate("@default", MainConfigurationWindow::SyntaxTextNotify) +
	tr("\n%&t - title (eg. New message) %&m - notification text (eg. Message from Jim), %&d - details (eg. message quotation),\n%&i - notification icon");

	configWindow->widget()->widgetById("Title")->setToolTip(tooltip);
	configWindow->widget()->widgetById("Syntax")->setToolTip(tooltip);

	configWindow->show();
	_activateWindow(configWindow);
}
