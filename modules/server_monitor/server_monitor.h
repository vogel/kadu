 /***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef SERVER_MONITOR_H
#define SERVER_MONITOR_H

#include "gui/windows/main-configuration-window.h"
#include "server_monitor_window.h"
class ActionDescription;
//class ServerMonitorWindow;

class ServerMonitor : public ConfigurationUiHandler
{
    Q_OBJECT

public:
    ServerMonitor(QWidget *parent = 0);
    ~ServerMonitor();
    virtual void mainConfigurationWindowCreated ( MainConfigurationWindow* mainConfigurationWindow );
private:
    ActionDescription *serverMonitorActionDescription;
    ServerMonitorWindow dialog;

private slots:
    void serverMonitorActionActivated(QAction *, bool);
};

extern ServerMonitor* serverMonitor;

#endif // SERVER_MONITOR_H
