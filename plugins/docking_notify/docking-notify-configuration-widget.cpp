/*
 * %kadu copyright begin%
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

#include <QtWidgets/QApplication>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QPushButton>

#include "core/application.h"
#include "gui/widgets/configuration/config-combo-box.h"
#include "gui/widgets/configuration/configuration-widget.h"
#include "gui/widgets/configuration/notify-group-box.h"
#include "gui/windows/configuration-window.h"
#include "gui/windows/main-configuration-window.h"
#include "misc/paths-provider.h"
#include "notification/notifier-configuration-data-manager.h"
#include "activate.h"
#include "debug.h"

#include "docking-notify-configuration-widget.h"

DockingNotifyConfigurationWidget::DockingNotifyConfigurationWidget(QWidget *parent)
	: NotifierConfigurationWidget(parent)
{
	QPushButton *configureButton = new QPushButton(tr("Configure"));
	connect(configureButton, SIGNAL(clicked()), this, SLOT(showConfigurationWindow()));

	QHBoxLayout *layout = new QHBoxLayout(this);
	layout->addWidget(configureButton);

	static_cast<NotifyGroupBox *>(parent)->addWidget(this);
}

void DockingNotifyConfigurationWidget::saveNotifyConfigurations()
{
}

void DockingNotifyConfigurationWidget::switchToEvent(const QString &event)
{
	kdebugf();

	currentNotificationEvent = event;
}

void DockingNotifyConfigurationWidget::showConfigurationWindow()
{
	NotifierConfigurationDataManager *dataManager = NotifierConfigurationDataManager::dataManagerForEvent(currentNotificationEvent);
	ConfigurationWindow *configWindow = new ConfigurationWindow("Qt4DockingNotificationEventConfiguration", tr("Tray icon balloon's look configuration"), "Qt4DockingNotify", dataManager);

	dataManager->configurationWindowCreated(configWindow);

	configWindow->widget()->appendUiFile(Application::instance()->pathsProvider()->dataPath() + QLatin1String("plugins/configuration/docking-notify.ui"));

	QString tooltip = QCoreApplication::translate("@default", MainConfigurationWindow::SyntaxTextNotify) +
	tr("\n%&t - title (eg. New message) %&m - notification text (eg. Message from Jim), %&d - details (eg. message quotation),\n%&i - notification icon");

	configWindow->widget()->widgetById("Title")->setToolTip(tooltip);
	configWindow->widget()->widgetById("Syntax")->setToolTip(tooltip);

	configWindow->show();
	_activateWindow(configWindow);
}

#include "moc_docking-notify-configuration-widget.cpp"
