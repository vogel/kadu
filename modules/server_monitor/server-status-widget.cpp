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
#include "notify/notification-manager.h"
#include "notify/notification.h"


#include "server-status-widget.h"

ServerStatusWidget::ServerStatusWidget(QString addr, quint16 watchedPort, QString name, QWidget *parent)
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
        this, SLOT( connected() )
    );

    connect(&tcpSocket, SIGNAL(error ( QAbstractSocket::SocketError)  ),
        this, SLOT(connectionError ( QAbstractSocket::SocketError ) )
     );

    connect( this, SIGNAL(statusChanged(QString,ServerStatus::ServerState)),
             this, SLOT(notifity(QString,ServerStatus::ServerState))
    );

    statusIcon = IconsManager::instance()->loadPixmap( "Invisible" );
    refreshIcon();
}

void ServerStatusWidget::emitNewStatus()
{
    tcpSocket.disconnectFromHost();
    if ( serverOldStatus == serverStatus ) return;

    emit statusChanged ( serverStatus, serverOldStatus );
    emit statusChanged ( address.toString(), serverStatus );

    update();
}


void ServerStatusWidget::connected()
{
    serverOldStatus = serverStatus;
    serverStatus = Available;
    statusIcon = IconsManager::instance()->loadPixmap( "Online" );
    emitNewStatus();
}

void ServerStatusWidget::connectionError ( QAbstractSocket::SocketError socketError )
{
    serverOldStatus = serverStatus;
    serverStatus = Unavailable;
    statusIcon = IconsManager::instance()->loadPixmap( "Offline" );

    tcpSocket.disconnectFromHost();
    emitNewStatus();
}

void ServerStatusWidget::paintEvent ( QPaintEvent * )
{
    QPainter painter( this );
    painter.drawPixmap(100,0, statusIcon);
}

void ServerStatusWidget::refreshIcon()
{
    kdebugf();
    qDebug() << tcpSocket.state();
//    if ( tcpSocket.state() == QAbstractSocket::UnconnectedState )
        tcpSocket.connectToHost( address, port, QIODevice::ReadOnly );
    kdebugf2();
}

void ServerStatusWidget::notifity ( QString adress , ServerStatusWidget::ServerState)
{
    Notification *notification = new Notification("serverMonitorChangeStatus",   QIcon() );

    notification->setDetails( QObject::tr("Server") +QObject::tr(" ")+
                              adress+QObject::tr(" ")+
                              QObject::tr("changed status to")+QObject::tr(" ")+
                              serverStateToString() );

    notification->setText("Server monitor");
    NotificationManager::instance()->notify( notification );
}

QString ServerStatusWidget::serverStateToString()
{
    switch ( serverStatus )
    {
        case Available:
            return QObject::tr( "Online" );
        break;

        case Unavailable:
            return QObject::tr("Unavailable");
        break;

        case Unknown:
            return QObject::tr("Unknown");
        break;

        case Empty:
            return QObject::tr("Empty");
        break;
    }
}
