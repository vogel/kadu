 /***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
*                                                                         *
 ***************************************************************************/

#include <QtCore/QDir>
#include <QtCore/QFile>
#include <QtGui/QGridLayout>
#include <QtNetwork/QHttp>

#include "core/core.h"
#include "configuration/configuration-file.h"
#include "debug.h"
#include "gui/actions/action-description.h"
#include "gui/widgets/configuration/configuration-widget.h"
#include "gui/windows/kadu-window.h"
#include "gui/windows/main-configuration-window.h"
#include "misc/misc.h"

#include "server-monitor-window.h"

ServerMonitorWindow::ServerMonitorWindow(QWidget *parent)
    : QScrollArea(parent),
      avalibleServers(0),
      unavalibleServers(0),
      unknownStatusServers(0),
      layout(0),
      scrollBarLayout(0)
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

    setWindowTitle( tr("Server monitor") );
}

ServerMonitorWindow::~ServerMonitorWindow()
{
    removeAllServer();
}

void ServerMonitorWindow::updateStats( ServerStatusWidget::ServerState newStatus, ServerStatusWidget::ServerState oldStatus )
{
    switch ( newStatus )
    {
        case ServerStatusWidget::Available:
            avalibleServers++;
        break;

        case ServerStatusWidget::Unavailable:
            unavalibleServers++;
        break;

        case ServerStatusWidget::Unknown:
            unknownStatusServers++;
        break;

        case ServerStatusWidget::Empty:
        break;
    }

    switch ( oldStatus )
    {
        case ServerStatusWidget::Available:
            avalibleServers--;
        break;

        case ServerStatusWidget::Unavailable:
            unavalibleServers--;
        break;

        case ServerStatusWidget::Unknown:
            unknownStatusServers--;
        break;

        case ServerStatusWidget::Empty:
        break;
    }
        stats.setText( tr("Avalible              ")+QString::number(avalibleServers)+"\n"+
                       tr("Unavailable        ")+QString::number(unavalibleServers)+"\n"
//                       +tr("Unknown        ")+QString::number(unknownStatusServers)
        );
}

void ServerMonitorWindow::readServerList()
{
    if ( layout == 0 ) delete layout;
    if ( scrollBarLayout == 0 ) delete scrollBarLayout;

    layout = new QGridLayout(this);
    scrollBarLayout = new QWidget(this);

    avalibleServers = 0;
    unavalibleServers = 0;
    removeAllServer();

    QFile serverFileList(serverFileListName);

    serverFileList.open( QIODevice::ReadOnly);

    if ( !serverFileList.isOpen() )
    {
        QLabel *labelInfo = new QLabel(tr("Cannot read server list!"));
        layout->addWidget(labelInfo, 1, 1 );
        stats.setText( tr("No information avalible"));
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
        ServerStatusWidget *serverStatusWidget = new ServerStatusWidget (addr,port.toInt(),name);
        servers.push_back( serverStatusWidget );

        int row = serverCounter;
        if ( serverCounter % 2 )
            --row;

        layout->addWidget(servers[serverCounter], row, (serverCounter) % 2 );

        connect (servers[serverCounter], SIGNAL( statusChanged (ServerStatusWidget::ServerState, ServerStatusWidget::ServerState)  ),
                 this, SLOT( updateStats (ServerStatusWidget::ServerState, ServerStatusWidget::ServerState) ));

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
    setConfiguration();
}

void ServerMonitorWindow::setConfiguration()
{
    kdebugf();

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

        QString url = config_file_ptr->readEntry( "serverMonitor", "serverListHost" );
        QString hostName;
        QString path;

        int index = url.indexOf("/");
        if ( index > 0 )
        {
            hostName = url.left(index);
            path = "/"+url.section("/",1);
        }
        else
        {
            hostName = url;
            path = "/serverslist.txt";
        }

        http = new QHttp ( hostName , 80, this );
        http->get( path, serverListBuffer );
        connect ( http, SIGNAL ( done (bool) ),
                    this, SLOT( downloadedServersList(bool) ));
    }
    else
    {
        serverFileListName = config_file_ptr->readEntry( "serverMonitor", "fileName", "kadu/modules/configuration/serverslist.txt");
        readServerList();
    }

    kdebugf2();
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

void ServerMonitorWindow::refreshList()
{
    foreach ( ServerStatusWidget* server, servers ) server->refreshIcon();
}

void ServerMonitorWindow::removeAllServer()
{
    foreach ( ServerStatusWidget *s, servers ) delete s;
    servers.clear();
}


