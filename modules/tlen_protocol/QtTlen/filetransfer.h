/*
 * %kadu copyright begin%
 * Copyright 2010 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2009 Bartłomiej Zimoń (uzi18@o2.pl)
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

#ifndef FILETRANSFER_H
#define FILETRANSFER_H

#include <QAbstractSocket>
#include <QThread>
#include <QTcpSocket>
#include <QTcpServer>
#include <QDomNode>
#include <QFile>
#include <QTimer>

//#include "ui_filetransferdialog.h"

class fileTransferThread: public QThread {
Q_OBJECT

public:
	enum PacketType {
		ConnectionRequest = 0x01,
		ConnectionRequestAck = 0x02,
		FileList = 0x32,
		FileListAck = 0x33,
		FileRequest = 0x34,
		FileData = 0x35,
		EndOfFile = 0x37,
		TransferAbort = 0x39
	};

	fileTransferThread(QString,int,QString,quint16,bool,QObject *parent=0);
	~fileTransferThread();
	void run();
	int id(){return rndid;}
	void transferingAccepted();
	void switchToSocketMode(QString, quint16);

public slots:
	void resetFilesData();
	void addFilesToSend();
	void send();

private slots:
	void readyRead();
	void error(QAbstractSocket::SocketError);
	void disconnected();
	void newConnection();
	void serverModeNewConnection();
	void estabilishFileReceiving();
	void estabilishFileTransfering( const QByteArray& );
	void sendFileChunk( const QByteArray& );
	void sendFileClose();
	void timeUpdate();

signals:
	//void updateTransferData(quint32,quint32,quint32,quint32,quint32,quint32,QTreeWidgetItem*);
	//void addListItem(QTreeWidgetItem*);
	void updateFilesData(quint32, quint32);
	void prepareTransfering();
	void readyToFileSend( const QByteArray& );

private:
	void parseFileList( const QByteArray& );
	void parseWriteData( const QByteArray& );
	void parseEndOfFile();
	void requestFile();
	void sendFileList();
	void sendFile( const QByteArray& );
	void switchToServerMode();

	QTcpSocket *socket;
	QTcpServer *server;
	QFile *current;
	QByteArray	stream,
			streamHeader;

	int rndid;
	quint16 port;

	QString	host,
		owner;

	quint32	streamSize;

	QTimer *t;

	//QList<QTreeWidgetItem*> fileMap;

	quint32	fc,
		fs,
		filesParsed,
		cfTime,
		allTime,
		currentFile,
		cfParsed,
		parsedSize,
		allSize;
};
/*
class fileTransferDialog: public QDialog {
Q_OBJECT

public:
	fileTransferDialog(int id, QString f, QString host=QString(), quint16 port=0, bool receiveMode=FALSE, QWidget *parent=0);
	int id(){return thread->id();}
	~fileTransferDialog();

	fileTransferThread *fileThread() { return thread;}

public slots:
	void addListItem(QTreeWidgetItem*);
	void updateFilesData(quint32, quint32);
	void prepareTransfering();

private slots:
	void updateTransferData(quint32 fc, quint32 filesParsed, quint32 allSize, quint32 parsedSize, quint32 allTime, quint32 cfTime, QTreeWidgetItem *it);
	void resetDialog();

private:
	Ui::downloadDialog ui;
	QPushButton *addFile, *clearList, *send, *abort, *goToFiles;
	QHBoxLayout *buttonLay;
	QLabel *statusLabel;
	fileTransferThread *thread;
	quint32 eta, avSpeed;
};
*/
#endif
