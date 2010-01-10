 /***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
#include <QtCore/QBuffer>
#include <QtGui/QPainter>
#include <QtCore/QString>
#include <QtNetwork/QTcpSocket>

#include "debug.h"
#include "icons-manager.h"

#include "server_status.h"

ServerStatus::ServerStatus(QString addr, quint16 watchedPort, QString name, QWidget *parent)
        : QWidget( parent ),
        serverStatus( Empty ),
        serverOldStatus( Empty ),
        address( addr ),
        port( watchedPort )
{
    setMinimumHeight( 17 );
    setMinimumWidth( 10 );
    labelAddress.setParent( this );
  
    if ( name.trimmed().length() > 0 )
        labelAddress.setText( name );
    else
        labelAddress.setText( addr );

    if ( port == 0 )
        port = 8074;

    connect(&tcpSocket, SIGNAL( connected() ),
        this, SLOT( connected() ) );

    connect(&tcpSocket, SIGNAL(error ( QAbstractSocket::SocketError)  ),
        this, SLOT(connectionError ( QAbstractSocket::SocketError ) )
     );
    statusIcon = IconsManager::instance()->loadPixmap( "Invisible" );
    refreshIcon();
}

void ServerStatus::emitNewStatus()
{
    tcpSocket.disconnectFromHost();
    if ( serverOldStatus == serverStatus ) return;

    emit statusChanged ( serverStatus, serverOldStatus );
    emit statusChanged ( address.toString(), serverStatus );

    update();
}


void ServerStatus::connected()
{
    serverOldStatus = serverStatus;
    serverStatus = Available;
    statusIcon = IconsManager::instance()->loadPixmap( "Online" );
    emitNewStatus();
}

void ServerStatus::connectionError ( QAbstractSocket::SocketError socketError )
{
    serverOldStatus = serverStatus;
    serverStatus = Unavailable;
    statusIcon = IconsManager::instance()->loadPixmap( "Offline" );

    tcpSocket.disconnectFromHost();
    emitNewStatus();
}

void ServerStatus::paintEvent ( QPaintEvent * )
{
    QPainter painter( this );
    painter.drawPixmap(100,0, statusIcon);
}

void ServerStatus::refreshIcon()
{
    kdebugf();
    qDebug() << tcpSocket.state();
    if ( tcpSocket.state() == QAbstractSocket::UnconnectedState )
        tcpSocket.connectToHost( address, port, QIODevice::ReadOnly );
    kdebugf2();
}
