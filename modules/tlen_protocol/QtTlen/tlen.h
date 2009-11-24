/***************************************************************************
 *   Copyright (C) 2004-2005 by Naresh [Kamil Klimek]                      *
 *   naresh@tlen.pl                                                        *
 *                                                                         *
 *   Copyright (C) 2008-2009 by uzi18 [Bartłomiej Zimoń]                   *
 *   uzi18@tlen.pl                                                         *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#ifndef TLEN_H
#define TLEN_H

#include <QObject>
#include <QDomNode>
#include <QMap>
#include <QTcpSocket>
#include <QDomDocument>
#include <QTimer>

//#include "filetransfer.h"

class tlen: public QObject {
Q_OBJECT

public:
	enum ConnectionState{	ConnectingToHub = 0,
				Connecting = 1,
				Connected = 2,
				ErrorDisconnected = 3,
				Disconnected = 4 };

	tlen(QObject *parent=0);

	bool isConnected();
	bool isConnecting();
	bool isDisconnected();

	QString strStatus() { return Status; }
	QString description() { return Descr; }

	QString uname() { return u; }
	void setUname(QString uname) { u = uname; }

	QString pass() { return p; }
	void setPass(QString pass) { p = pass; }

	bool isSecureConn() { return Secure; }
	void setSecureConn(bool secure) { Secure = secure; }

	bool reconnect() { return Reconnect; }
	void setReconnect(bool reconnect) { Reconnect = reconnect; }

	QString decode(const QByteArray&);
	QString decode(const QString&);
	QByteArray encode(const QString&);

	QString localAddress();

	//QMap<QString, fileTransferDialog*> fTransferMap;

	QString token;
	// create class or something for this config
	QString mmBase() { return MiniMailBase; }

	QString MiniMailBase;
	QString MiniMailMsg;
	QString MiniMailMsgMethod;
	QString MiniMailIndex;
	QString MiniMailIndexMethod;
	QString MiniMailLogin;
	QString MiniMailLoginMethod;
	QString MiniMailLogout;
	QString MiniMailLogoutMethod;
	QString MiniMailCompose;
	QString MiniMailComposeMethod;
	QString MiniMailAvatarGet;
	QString MiniMailAvatarGetMethod;
	QString MiniMailAvatarUpload;
	QString MiniMailAvatarUploadMethod;
	QString MiniMailAvatarRemove;
	QString MiniMailAvatarRemoveMethod;

public slots:
	void openConn();
	void closeConn();
	void writeMsg(QString msg, QString to);
	void sendAlarm(QString to);
	void chatNotify(QString to, bool t);
	void rosterRequest();
	// tlen configuration request
	void tcfgRequest();
	// pobiera dane zapisane w katalogu publicznym
	void getPubDirInfoRequest();

	// "available","chat","away","xa","dnd","invisible","unavailable"
	void setStatus(QString status);
	void setStatusDescr(QString status,QString description);

	// add Contact
	void addItem(QString jid, QString name, QString group, bool subscribe);
	// remove Contact
	void remove(QString jid);

	void receiveFile(QString,QString,bool);
	bool write(const QDomDocument &d);

private slots:
	void socketConnected();
	void socketReadyRead();
	void socketDisconnected();

	// send subscription to user
	void authorize(QString to, bool subscribe);
	// send status and connect or disconnect if needed
	void writeStatus();
	// event parse
	void event(QDomNode n);
	// keep-alive session every 50-60 s.
	void sendPing();
	// tlen configuration received
	void tcfgReceived(QDomElement &n);

signals:
	void presenceDisconnected();
	void itemReceived(QString jid, QString name, QString subscription, QString group, bool sort);
	void presenceChanged(QString from, QString status, QString description);
	void authorizationAsk(QString);
	void removeItem(QString);
	void avatarReceived(QString jid, QString type, QString md5);
	void pubdirReceived(QDomNodeList n);

	void sortRoster();

	void chatMsgReceived(QDomNode n);

	void clearRosterView();
	void tlenLoggedIn();
	void statusChanged(); // FOR GUI TO UPDATE ICONS
	void statusUpdate();	// FOR TLEN TO WRITE STATUS
	void eventReceived(QDomNode n);

	void chatNotify(QString from, QString type);
private:
	bool tlenLogin();
	bool sort;
	bool Secure;
	bool Reconnect;

	QTimer *ping;

	QDomDocument *tmpDoc;

	QByteArray stream;

	int state;

	QString	u,
		p,
		sid,
		hostname,
		Status,
		Descr;

	quint16 hostport;
	QTcpSocket *socket;
};

extern tlen *Tlen;
#endif
