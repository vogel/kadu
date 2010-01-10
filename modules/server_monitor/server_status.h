 /***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef SERVERSTATUS_H
#define SERVERSTATUS_H

#include <QtCore/QBuffer>
#include <QtGui/QLabel>
#include <QtGui/QWidget>

#include <QtNetwork/QTcpSocket>
#include <QtNetwork/QHostAddress>

class ServerStatus : public QWidget
{

Q_OBJECT

    public:

    enum ServerState {
      Available,
      Unavailable,
      Unknown,
      Empty
    };

public:
    QPixmap statusIcon;

    ServerState serverStatus;
    ServerState serverOldStatus;
    ServerStatus(QString addr, quint16 watchedPort, QString name, QWidget *parent=0);

public slots:
    void refreshIcon();

private:
    QHostAddress address;
    QLabel labelAddress;
    quint16 port;
    QAbstractSocket::SocketState prevSocketState;
    QTcpSocket tcpSocket;

    void emitNewStatus();
    void paintEvent ( QPaintEvent * event );


private slots:
    void connected();
    void connectionError ( QAbstractSocket::SocketError socketError );

signals:
    void statusChanged ( ServerStatus::ServerState, ServerStatus::ServerState );
    void statusChanged ( QString, ServerStatus::ServerState );
};

#endif // SERVERSTATUS_H
