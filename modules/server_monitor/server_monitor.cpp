 /***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
*                                                                         *
 ***************************************************************************/
#include<QCheckBox>

#include "debug.h"
#include "core/core.h"
#include "configuration/configuration-file.h"
#include "gui/actions/action-description.h"
#include "gui/windows/kadu-window.h"
#include "gui/windows/main-configuration-window.h"
#include "gui/widgets/configuration/configuration-widget.h"
#include "misc/misc.h"

#include "server_monitor.h"

extern "C" KADU_EXPORT int server_monitor_init(bool firstLoad)
{
    kdebugf();
    serverMonitor = new ServerMonitor();
    MainConfigurationWindow::instance()->registerUiFile(dataPath("kadu/modules/configuration/server_monitor.ui"));
    MainConfigurationWindow::registerUiHandler(serverMonitor);
    if ( firstLoad )
    {
        config_file_ptr->addVariable( "serverMonitor", "autorefresh", true );
        config_file_ptr->addVariable( "serverMonitor", "fileName", "kadu/modules/configuration/serverslist.txt" );
        config_file_ptr->addVariable( "serverMonitor", "serverListHost", "sifaka.pl" );
        config_file_ptr->addVariable( "serverMonitor", "useListFromServer", true );
        config_file_ptr->addVariable( "serverMonitor", "timerInterval", 5 );
        config_file_ptr->addVariable( "serverMonitor", "showResetButton", false );
    }

    kdebugf2();
    return 0;
}

extern "C" KADU_EXPORT void server_monitor_close()
{
    kdebugf();
    MainConfigurationWindow::instance()->unregisterUiFile(dataPath("kadu/modules/configuration/server_monitor.ui"));
    MainConfigurationWindow::instance()->unregisterUiHandler(serverMonitor);
    delete serverMonitor;
    serverMonitor = NULL;
    kdebugf2();
}

ServerMonitor::ServerMonitor(QWidget *parent)
    : QObject(parent)

{
    serverMonitorActionDescription = new ActionDescription(
        this,ActionDescription::TypeMainMenu, "serverMonitorAction",
        this, SLOT(serverMonitorActionActivated(QAction *, bool)),
        "Online", tr("Server's monitor")
    );
    Core::instance()->kaduWindow()->insertMenuActionDescription( serverMonitorActionDescription,KaduWindow::MenuKadu,6 );
}

void ServerMonitor::serverMonitorActionActivated(QAction *, bool)
{
   dialog.show();
}

void ServerMonitor::mainConfigurationWindowCreated ( MainConfigurationWindow* mainConfigurationWindow )
{
    kdebugf();
    connect(mainConfigurationWindow->widget()->widgetById("serverMonitor/useListFromServer"), SIGNAL(toggled(bool)),
        mainConfigurationWindow->widget()->widgetById("serverMonitor/hostNameEdit"), SLOT(setEnabled(bool)));

    connect(mainConfigurationWindow->widget()->widgetById("serverMonitor/useListFromServer"), SIGNAL(toggled(bool)),
        mainConfigurationWindow->widget()->widgetById("serverMonitor/fileSelect"), SLOT(setDisabled(bool)));
    kdebugf2();
}

ServerMonitor::~ServerMonitor()
{
    Core::instance()->kaduWindow()->removeMenuActionDescription( serverMonitorActionDescription );
}
ServerMonitor *serverMonitor;
