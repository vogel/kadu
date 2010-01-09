 /***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
*                                                                         *
 ***************************************************************************/

#include <QDir>
#include <QFile>
#include <QRegExpValidator>

#include "debug.h"
#include "core/core.h"
#include "configuration/configuration-file.h"
#include "gui/actions/action-description.h"
#include "gui/windows/kadu-window.h"
#include "gui/windows/main-configuration-window.h"
#include "gui/widgets/configuration/configuration-widget.h"
#include "misc/misc.h"

#include "server_monitor_window.h"

ServerMonitorWindow::ServerMonitorWindow(QWidget *parent)
    : QScrollArea(parent),
      avalibleServers(0),
      unavalibleServers(0),
      unknownStatusServers(0)
{
    buttonRefresh.setParent( this );
    buttonRefresh.setGeometry(420, 75, 60, 25);
    buttonRefresh.setText( tr("Refresh"));

    connect (&buttonRefresh, SIGNAL ( clicked(bool) ), this,       SLOT ( refreshList() ) );
    connect (&refreshTimer,  SIGNAL ( timeout () ),    this,       SLOT ( refreshList() ) );
    connect (&refreshTimer,  SIGNAL ( timeout () ), &refreshTimer, SLOT ( start() ) );

    setConfiguration();

    stats.setParent( this );
    stats.setGeometry(420, 20, 150, 50 );
    stats.setText( tr("No information avalible"));
}

void ServerMonitorWindow::updateStats( ServerStatus::ServerState newStatus, ServerStatus::ServerState oldStatus )
{
    switch ( newStatus )
    {
        case ServerStatus::Available:
            avalibleServers++;
        break;

        case ServerStatus::Unavailable:
            unavalibleServers++;
        break;

        case ServerStatus::Unknown:
            unknownStatusServers++;
        break;

        case ServerStatus::Empty:
        break;
    }

    switch ( oldStatus )
    {
        case ServerStatus::Available:
            avalibleServers--;
        break;

        case ServerStatus::Unavailable:
            unavalibleServers--;
        break;

        case ServerStatus::Unknown:
            unknownStatusServers--;
        break;

        case ServerStatus::Empty:
        break;
    }
        stats.setText( tr("Avalible              ")+QString::number(avalibleServers)+"\n"+
                       tr("Unavailable        ")+QString::number(unavalibleServers)+"\n"
//                       +tr("Unknown        ")+QString::number(unknownStatusServers)
        );
}

void ServerMonitorWindow::readServerList()
{
    QGridLayout *layout = new QGridLayout(this);
    QWidget *scrollBarLayout = new QWidget(this);

    avalibleServers = 0;
    unavalibleServers = 0;
    servers.clear();

    QFile serverFileList(serverFileListName);

    serverFileList.open( QIODevice::ReadOnly);

    if ( !serverFileList.isOpen() )
    {
        QLabel *labelInfo = new QLabel(tr("Cannot read server list!"));
        layout->addWidget(labelInfo, 1, 1 );
        kdebugm ( KDEBUG_ERROR, serverFileListName.toAscii() );
        setLayout( layout );
        return;
    }

    int serverCounter = 0;
    while ( !serverFileList.atEnd() )
    {

        QString line = serverFileList.readLine();
        QStringList lineSpilted = line.split(':');

        QString addr = lineSpilted[0];
        QString port = "";
        QString name = "";

        if ( lineSpilted.length() > 1 )
        {
            port = lineSpilted[1];

            if ( lineSpilted.length() > 2 )
                name = lineSpilted[2];
        }
        ServerStatus *serverStatus = new ServerStatus (addr,port.toInt(),name);
        servers.push_back( serverStatus );

        int row = serverCounter;
        if ( serverCounter % 2 )
            --row;

        layout->addWidget(servers[serverCounter], row, (serverCounter) % 2 );

        connect (servers[serverCounter], SIGNAL( statusChanged (ServerStatus::ServerState, ServerStatus::ServerState)  ),
                 this, SLOT( updateStats (ServerStatus::ServerState, ServerStatus::ServerState) ));

        serverCounter++;
    }
    serverFileList.close();

    scrollBarLayout->setLayout( layout );
    scrollBarLayout->setSizePolicy( QSizePolicy::Maximum, QSizePolicy::Maximum );
    scrollBarLayout->setFixedWidth(500);
    setWidget(scrollBarLayout);
    setFixedWidth( 600 );
}


void ServerMonitorWindow::configurationUpdated()
{
    kdebugf();

    serverFileListName = config_file_ptr->readEntry( "serverMonitor", "fileName", "kadu/modules/configuration/serverslist.txt");
    readServerList();

    if ( config_file_ptr->readBoolEntry( "serverMonitor", "showResetButton",false) )
        buttonRefresh.show();
    else
        buttonRefresh.hide();

    if ( config_file_ptr->readBoolEntry( "serverMonitor", "autorefresh", true) )
        refreshTimer.start( 60000 * config_file_ptr->readNumEntry( "serverMonitor", "timerInterval", 5));
    else
        refreshTimer.stop();

    setConfiguration();
    kdebugf2();
}

void ServerMonitorWindow::setConfiguration()
{
    if ( !config_file_ptr->readBoolEntry( "serverMonitor", "showResetButton", false) )
        buttonRefresh.hide();
    else
        buttonRefresh.show();

    if ( config_file_ptr->readBoolEntry( "serverMonitor", "autorefresh", true) )
        refreshTimer.start( 60000 * config_file_ptr->readNumEntry( "serverMonitor", "timerInterval", 5));
    else
        refreshTimer.stop();

   if ( config_file_ptr->readBoolEntry( "serverMonitor", "useListFromServer", false) ) // if ( config_file_ptr->readBoolEntry( "serverMonitor", "useListFromServer" ), true )
    {

        serverListBuffer = new QBuffer();
        http = new QHttp ( config_file_ptr->readEntry( "serverMonitor", "serverListHost" ), 80, this );
        http->get( "/serverslist.txt", serverListBuffer );
        connect ( http, SIGNAL ( done (bool) ),
                    this, SLOT( downloadedServersList(bool) ));
    }
    else
    {
        serverFileListName = config_file_ptr->readEntry( "serverMonitor", "fileName", "kadu/modules/configuration/serverslist.txt");
        readServerList();
    }
}

void ServerMonitorWindow::downloadedServersList( bool err )
{
    kdebugf();

    if ( err )
    {
        kdebugm( KDEBUG_WARNING, "Cannont download server's list!" );
        serverFileListName = config_file_ptr->readEntry( "serverMonitor", "fileName", "kadu/modules/configuration/serverslist.txt");
        return;
    }

    QFile fileList(QDir::tempPath()+"/serverslist.txt");
    fileList.open( QIODevice::WriteOnly);
    fileList.write( serverListBuffer->buffer() );
    fileList.close();

    serverFileListName = QDir::tempPath()+"/serverslist.txt";
    readServerList();

//    disconnect ( http, SIGNAL ( done (bool) ),
//        this, SLOT( downloadedServersList(bool) ));
    delete http;
    delete serverListBuffer;

    kdebugf2();
}



ServerMonitorWindow::~ServerMonitorWindow()
{

}

