/*
 * %kadu copyright begin%
 * Copyright 2009, 2010 Ziemniak (jziemkiewicz@gmail.com)
 * Copyright 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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
#include<QtGui/QCheckBox>

#include "core/core.h"
#include "configuration/configuration-file.h"
#include "gui/actions/action-description.h"
#include "gui/widgets/configuration/configuration-widget.h"
#include "gui/windows/kadu-window.h"
#include "gui/windows/main-configuration-window.h"
#include "misc/misc.h"
#include "notify/notification-manager.h"
#include "notify/notify-event.h"
#include "activate.h"
#include "debug.h"
#include "icons-manager.h"
#include "server-monitor-window.h"

#include "server-monitor.h"

	NotifyEvent* ServerMonitor::notifyEvent=NULL;

extern "C" KADU_EXPORT int server_monitor_init(bool firstLoad)
{
	kdebugf();
	serverMonitor = new ServerMonitor();
	MainConfigurationWindow::registerUiFile(dataPath("kadu/modules/configuration/server-monitor.ui"));
	MainConfigurationWindow::registerUiHandler(serverMonitor);

	if (firstLoad)
	{
		config_file.addVariable("serverMonitor", "autorefresh", true);
		config_file.addVariable("serverMonitor", "fileName", dataPath("kadu/modules/configuration/serverslist.txt"));
		config_file.addVariable("serverMonitor", "useGaduServersList", true);
		config_file.addVariable("serverMonitor", "timerInterval", 5);
		config_file.addVariable("serverMonitor", "showResetButton", false);
	}

	ServerMonitor::notifyEvent = new NotifyEvent("serverMonitorChangeStatus", NotifyEvent::CallbackNotRequired, "Server Monitor");
	NotificationManager::instance()->registerNotifyEvent(ServerMonitor::notifyEvent);

	kdebugf2();
	return 0;
}

extern "C" KADU_EXPORT void server_monitor_close()
{
	kdebugf();

	MainConfigurationWindow::unregisterUiFile(dataPath("kadu/modules/configuration/server-monitor.ui"));
	MainConfigurationWindow::unregisterUiHandler(serverMonitor);

	NotificationManager::instance()->unregisterNotifyEvent(ServerMonitor::notifyEvent);
	delete serverMonitor;
	serverMonitor = NULL;
	kdebugf2();
}

ServerMonitor::ServerMonitor(QWidget *parent) :
		QObject(parent), Dialog(0)
{
	ServerMonitorActionDescription = new ActionDescription(
			this,ActionDescription::TypeMainMenu, "serverMonitorAction",
			this, SLOT(serverMonitorActionActivated(QAction *, bool)),
			"protocols/gadu-gadu/online", tr("Server's Monitor"));
	Core::instance()->kaduWindow()->insertMenuActionDescription(ServerMonitorActionDescription, KaduWindow::MenuTools, 7);

	Dialog = new ServerMonitorWindow();
}

void ServerMonitor::serverMonitorActionActivated(QAction* ,bool)
{
	Dialog->show();
	_activateWindow(Dialog);
}

void ServerMonitor::mainConfigurationWindowCreated(MainConfigurationWindow* mainConfigurationWindow)
{
	kdebugf();
	connect(mainConfigurationWindow->widget()->widgetById("serverMonitor/useGaduServersList"), SIGNAL(toggled(bool)),
			mainConfigurationWindow->widget()->widgetById("serverMonitor/fileSelect"), SLOT(setDisabled(bool)));
	kdebugf2();
}

ServerMonitor::~ServerMonitor()
{
	Core::instance()->kaduWindow()->removeMenuActionDescription(ServerMonitorActionDescription);
	delete Dialog;
}

ServerMonitor *serverMonitor;
