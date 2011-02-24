/*
 * %kadu copyright begin%
 * Copyright 2010 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2010 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#include <QPushButton>
#include <QHBoxLayout>
#include <QStringList>
#include <QFileDialog>
#include <QHostInfo>

#include <QDebug>

#include "filetransfer.h"
#include "settings.h"
#include "sha1.h"
#include "tlen.h"

void hexdump(unsigned char *buf, size_t len) {
	qDebug()<<"Hex:"<<len<<"bytes";
	size_t i;
	for (i = 0; i < len; i++) {
		printf("%02x ", buf[i]);
		if ((i + 1) % 8 == 0 || i+1==len) {
			printf("\n");
		} else if ((i + 1) % 4 == 0) {
			printf("\t");
		}
	}
}
/*
fileTransferDialog::fileTransferDialog(int i, QString f, QString host, quint16 port, bool receiveMode, QWidget *parent): QDialog(parent) {
	setAttribute(Qt::WA_DeleteOnClose);
	ui.setupUi(this);
	buttonLay = new QHBoxLayout();
	buttonLay->setMargin(0);
	ui.bottomFrame->setContentsMargins(0,0,0,0);
	statusLabel = new QLabel;
	buttonLay->addWidget(statusLabel);
	buttonLay->addStretch();
	abort = new QPushButton(tr("Abort"));
	send = new QPushButton(tr("Send"));

	QStringList labelList;
	labelList<<tr("Filename")<<tr("Speed")<<tr("Elapsed")<<tr("Estimated")<<tr("Progress")<<tr("Size")<<tr("Path")<<"RealSize";
	ui.fileListTreeWidget->setHeaderLabels(labelList);
	ui.fileListTreeWidget->setColumnHidden(7,TRUE);

	setWindowIcon( QIcon(":logo") );

	thread = new fileTransferThread(f,i,host,port,receiveMode,this);
	connect( thread, SIGNAL( addListItem( QTreeWidgetItem* ) ), this, SLOT( addListItem( QTreeWidgetItem* ) ) );
	connect( thread, SIGNAL( updateFilesData(quint32, quint32) ), this, SLOT( updateFilesData(quint32, quint32) ) );
	connect( thread, SIGNAL( prepareTransfering() ), this, SLOT( prepareTransfering() ) );
	connect( thread, SIGNAL( updateTransferData(quint32,quint32,quint32,quint32,quint32,quint32,QTreeWidgetItem*) ), this, SLOT( updateTransferData(quint32,quint32,quint32,quint32,quint32,quint32,QTreeWidgetItem*) ) );

	if(!receiveMode) {
		addFile = new QPushButton(tr("Add file to send"));
		connect( addFile, SIGNAL( clicked() ), thread, SLOT( addFilesToSend() ) );
		clearList = new QPushButton(tr("Clear list"));
		connect( clearList, SIGNAL( clicked() ), this, SLOT( resetDialog() ) );
		send->setEnabled(FALSE);
		connect( send, SIGNAL( clicked() ), thread, SLOT( send() ) );
		buttonLay->addWidget(addFile);
		buttonLay->addWidget(clearList);
		buttonLay->addWidget(send);
		abort->setEnabled(FALSE);
		setWindowTitle( tr("Sending files to %1").arg( f+"@tlen.pl" ) );
	} else {
		setWindowTitle( QString("Receiving files from %1").arg( f+"@tlen.pl" ) );
	}
	buttonLay->addWidget(abort);
	ui.bottomFrame->setLayout(buttonLay);
	thread->run();
	setWindowFlags(Qt::Window);
}

fileTransferDialog::~fileTransferDialog() {
	delete thread;
}

void fileTransferDialog::resetDialog() {
	thread->resetFilesData();
	ui.fileCountLabel->setText( tr("File count: 0") );
	ui.filesRemainedLabel->setText( tr("Files remained: 0 (0 kB)") );
	ui.filesSizeLabel->setText( tr("Files size: 0 kB") );
	ui.fileListTreeWidget->clear();
	send->setEnabled(FALSE);
}

void fileTransferDialog::updateTransferData(quint32 fc, quint32 filesParsed, quint32 allSize, quint32 parsedSize, quint32 allTime, quint32 cfTime, QTreeWidgetItem *it) {
	ui.filesRemainedLabel->setText( tr("Files remained: %1 (%2 kB)").arg(fc-filesParsed).arg( (allSize-parsedSize)/1024 ) );
	if(allTime) {
		avSpeed = (parsedSize/allTime)/1024;
		ui.avarageSpeedLabel->setText( tr("Avarage speed: %1 kB/s").arg( avSpeed ) );
		eta = (allSize-parsedSize)/(avSpeed*1024);
		ui.etaLabel->setText( tr("Estimated time of arrival: %1:%2:%3").arg(eta/3600).arg( (eta%3600)/60 ).arg(eta%60) );
	}

	ui.elapsedTimeLabel->setText( tr("Elapsed time: %1:%2:%3").arg(allTime/3600).arg((allTime%3600)/60).arg(allTime%60) );
	it->setText(2, QString("%1:%2:%3").arg( cfTime/3600 ).arg( (cfTime%3600)/60 ).arg( cfTime%60 ) );
}

void fileTransferDialog::updateFilesData(quint32 fc, quint32 allSize) {
	ui.fileCountLabel->setText( tr("File count: %1").arg(fc) );
	ui.filesRemainedLabel->setText( tr("Files remained: %1 (%2 kB)").arg(fc).arg(allSize/1024) );
	ui.filesSizeLabel->setText( tr("Files size: %1 kB").arg(allSize/1024) );
	send->setEnabled(TRUE);
}

void fileTransferDialog::addListItem( QTreeWidgetItem *item ) {
	ui.fileListTreeWidget->addTopLevelItem(item);
}

void fileTransferDialog::prepareTransfering() {
	send->setEnabled(FALSE);
	clearList->setEnabled(FALSE);
	addFile->setEnabled(FALSE);
}

void fileTransferThread::transferingAccepted() {
//send: <f t="%receiver%" i="274851192" e="6" a="%sender_ip%" p="%sender_port%"/>
	qDebug()<<server->listen(QHostAddress::Any, settings->profileValue("network/filetransfer/port").toUInt());
	qDebug()<<server->errorString();
	QDomDocument doc;
	QDomElement f=doc.createElement("f");
	f.setAttribute( "t", owner );
	f.setAttribute( "i", rndid );
	f.setAttribute( "e", "6" );
	f.setAttribute( "p", settings->profileValue("network/filetransfer/port").toUInt() );
	f.setAttribute( "a", Tlen->localAddress() );
	doc.appendChild(f);
	Tlen->write(doc);
}
*/
fileTransferThread::~fileTransferThread() {
	if( server && server->isListening() )
		server->close();

	terminate();
}

fileTransferThread::fileTransferThread(QString f,int i,QString h,quint16 p,bool receiveMode,QObject *parent): QThread(parent) {
	port=p;
	host=h;
	owner=f;
	rndid=i;
	current = new QFile(this);

	if(receiveMode) {
		socket = new QTcpSocket(this);
		connect( socket, SIGNAL( readyRead() ), this, SLOT( readyRead() ) );
		connect( socket, SIGNAL( error(QAbstractSocket::SocketError) ), this, SLOT( error(QAbstractSocket::SocketError) ) );
		connect( socket, SIGNAL( disconnected() ), this, SLOT( disconnected() ) );
		connect( socket, SIGNAL( connected() ), this, SLOT( estabilishFileReceiving() ) );
		socket->connectToHost(h,p);
		server = 0;
	} else {
		server = new QTcpServer(this);
		connect( server, SIGNAL( newConnection() ), this, SLOT( newConnection() ) );
		connect( this, SIGNAL( readyToFileSend( const QByteArray& ) ), this, SLOT( sendFileChunk( const QByteArray& ) ) );
	}

	t=new QTimer(this);
	connect(t, SIGNAL( timeout() ), this, SLOT( timeUpdate() ) );

	currentFile=1;
	allSize=0;
	allTime=0;
	fc=0;
}

void fileTransferThread::timeUpdate() {
	allTime++;
	cfTime++;

	emit updateTransferData(fc, filesParsed, allSize, parsedSize, allTime, cfTime, fileMap.at(currentFile));
}

void fileTransferThread::addFilesToSend() {
	QStringList flist = QFileDialog::getOpenFileNames( static_cast<QWidget*>(parent()), tr("Choose files to send"), QDir::homePath() );

	if( flist.count() ) {
		QStringListIterator it(flist);
		while(it.hasNext()) {
			QString f=it.next();
			QFileInfo fifo(f);
			QStringList info;
			info<<fifo.fileName()<<""<<""<<""<<""<<QString("%L1 kB").arg( (float)qRound( ( (float)fifo.size()/1024 )*10 )/10 )<<fifo.absolutePath()<<QString("%1").arg( fifo.size() );
			fc++;
			allSize+=fifo.size();
			QTreeWidgetItem *item = new QTreeWidgetItem(info);
			fileMap<<item;
			emit addListItem(item);
		}
		emit updateFilesData(fc, allSize);
	}
}

void fileTransferThread::run() {
	start();
}

void fileTransferThread::readyRead() {
	while( socket->bytesAvailable() ) {
		if(stream.isEmpty()) {
			streamHeader=socket->read(8-streamHeader.size());
			if(streamHeader.size() < 8)
				return;
			memcpy(&streamSize, streamHeader.mid(4).data(), 4);
		}
		stream+=socket->read( streamSize-stream.size() );

		if( streamSize > (quint32)stream.size() ) //packet is not full
			return;

		if( streamHeader.at(0) == ConnectionRequest ) {
			qDebug()<<"ConnectionRequest";
			estabilishFileTransfering( stream );
		} else if( streamHeader.at(0) == ConnectionRequestAck) {
			qDebug()<<"ConnectionRequestAck";
		} else if( streamHeader.at(0) == FileList) {
			qDebug()<<"FileList";
			parseFileList( stream );
		} else if( streamHeader.at(0) == FileListAck) {
			qDebug()<<"FileListAck";
		} else if( streamHeader.at(0) == FileRequest) {
			qDebug()<<"FileRequest";
			sendFile( stream );
		} else if( streamHeader.at(0) == FileData) {
			qDebug()<<"FileData";
			parseWriteData( stream );
		} else if( streamHeader.at(0) == EndOfFile) {
			qDebug()<<"EndOfFile";
			parseEndOfFile();
		} else if( streamHeader.at(0) == TransferAbort) {
			qDebug()<<"TransferAbort";
		} else {
			qDebug()<<"Unknown type";
		}
		stream.clear();
		streamHeader.clear();
	}
}

void fileTransferThread::error(QAbstractSocket::SocketError e) {
	switch(e) {
	case QAbstractSocket::HostNotFoundError:
	case QAbstractSocket::ConnectionRefusedError:
		switchToServerMode();

	default:
		qDebug()<<"Error: "<<e;
	}
}

void fileTransferThread::switchToSocketMode(QString h, quint16 p) {
	socket = new QTcpSocket(this);
	connect( socket, SIGNAL( readyRead() ), this, SLOT( readyRead() ) );
	connect( socket, SIGNAL( error(QAbstractSocket::SocketError) ), this, SLOT( error(QAbstractSocket::SocketError) ) );
	connect( socket, SIGNAL( disconnected() ), this, SLOT( disconnected() ) );
	connect( socket, SIGNAL( connected() ), this, SLOT( estabilishFileReceiving() ) );
	socket->connectToHost(h,p);
}

void fileTransferThread::switchToServerMode() {
	server = new QTcpServer(this);
	connect( server, SIGNAL( newConnection() ), this, SLOT( serverModeNewConnection() ) );
	server->listen(QHostAddress::Any, settings->profileValue("network/filetransfer/port").toUInt());
	QDomDocument doc;
	QDomElement f=doc.createElement("f");
	f.setAttribute( "t", owner );
	f.setAttribute( "i", rndid );
	f.setAttribute( "e", "7" );
	f.setAttribute( "p", settings->profileValue("network/filetransfer/port").toUInt() );
	f.setAttribute( "a", Tlen->localAddress() );
	doc.appendChild(f);
	Tlen->write(doc);
}

void fileTransferThread::serverModeNewConnection() {
	qDebug()<<"New connection!";
	if( server->hasPendingConnections() ) {
		socket = server->nextPendingConnection();
		connect( socket, SIGNAL( readyRead() ), this, SLOT( readyRead() ) );
		connect( socket, SIGNAL( error(QAbstractSocket::SocketError) ), this, SLOT( error(QAbstractSocket::SocketError) ) );
		connect( socket, SIGNAL( disconnected() ), this, SLOT( disconnected() ) );
		estabilishFileReceiving();
	}
}

void fileTransferThread::disconnected() {
	qDebug()<<"Disconnected!";
}

void fileTransferThread::estabilishFileReceiving() {
	qDebug()<<"Estabilishing file receiving";
	char head[8];
	char packet[28];
	quint32 tmp;
	char str[300];
	tmp = ConnectionRequest;
	memcpy(&packet, &tmp, 4);
	memcpy(&head, &tmp, 4);
	memcpy(&packet[4], &rndid, 4);
	char *hash;
	char username[128];
	sprintf(username, settings->profileValue("user/login").toByteArray().data() );
	snprintf(str, sizeof(str), "%08X%s%d", rndid, username, rndid );
	hash=TlenSha1( str, strlen(str) );
	memcpy(&packet[8], hash, 20);
	tmp=28;
	memcpy(&head[4], &tmp, 4);
	socket->write( head, sizeof(head) );
	socket->write( packet, sizeof(packet) );
}

void fileTransferThread::estabilishFileTransfering( const QByteArray &s ) {
	qDebug()<<"Estabilishing file transfering";
	char packet[28];
	quint32 tmp;
	char str[300];
	tmp = ConnectionRequest;
	memcpy(&packet, &tmp, 4);
	memcpy(&packet[4], &rndid, 4);
	char *hash;
	char username[128];
	sprintf(username, settings->profileValue("user/login").toByteArray().data() );
	snprintf(str, sizeof(str), "%08X%s%d", rndid, username, rndid );
	hash=TlenSha1( str, strlen(str) );
	memcpy(&packet[8], hash, 20);
	if( packet == s ) {
		qDebug()<<"Hash control";
		server->close();
		char head[8];
		char ack[4];
		quint32 tmp;
		memcpy(&ack, &rndid, 4);
		tmp = ConnectionRequestAck;
		memcpy(&head, &tmp, 4);
		tmp=4;
		memcpy(&head[4], &tmp, 4);
		socket->write(head, 8);
		socket->write(ack, 4);
		sendFileList();
	} else {
		socket->close();
	}
}

void fileTransferThread::sendFileList() {
	parsedSize=0;
	filesParsed=0;
	char head[8];
	char packet[4+260*fileMap.count()];
	memset( &packet, 0, sizeof(packet) );
	quint32 tmp=FileList;
	memcpy(&head, &tmp, 4);
	tmp=fileMap.count();
	memcpy(&packet, &tmp, 4);
	for(int i=0; i<fileMap.count(); ++i) {
		QTreeWidgetItem *it=fileMap.at(i);
		tmp=it->text(7).toInt();
		memcpy(&packet[4+260*i], &tmp, 4);
		sprintf( &packet[8+260*i], it->data(0, Qt::DisplayRole).toByteArray().data() );
	}
	tmp=sizeof(packet);
	memcpy(&head[4], &tmp, 4);
	socket->write(head, 8);
	socket->write( packet, sizeof(packet) );
}

void fileTransferThread::parseWriteData( const QByteArray &data ) {
	qDebug()<<"Writing chunk to file"<<current->fileName()<<"Size:"<<current->write( data.mid(8) ); //skip file offset
	parsedSize+=streamSize-8;
	cfParsed+=streamSize-8;
	emit updateTransferData(fc, filesParsed, allSize, parsedSize, allTime, cfTime, fileMap.at(currentFile));
}

void fileTransferThread::parseEndOfFile() {
	qDebug()<<"Closing file"<<current->fileName();
	current->close();
	fileMap.at(currentFile)->setIcon(0, QIcon(":online"));
	++filesParsed;
	if(filesParsed==fc)
		t->stop();
	emit updateTransferData(fc, filesParsed, allSize, parsedSize, allTime, cfTime, fileMap.at(currentFile));
	++currentFile;
	requestFile();
}

void fileTransferThread::requestFile() {
	if(currentFile < fc) {
		if(!t->isActive())
			t->start(1000);
		cfTime = 0;
		cfParsed=0;
		qDebug()<<"File request";
		char fpacket[20];
		quint32 tmp=FileRequest;
		memcpy(&fpacket, &tmp,4);
		tmp=12;
		memcpy(&fpacket[4], &tmp, 4);
		memcpy(&fpacket[8], &currentFile, 4);
		tmp=0;
		memcpy(&fpacket[12], &tmp, 4);
		memcpy(&fpacket[16], &tmp, 4);
		QTreeWidgetItem *it = fileMap.at(currentFile);
		current->setFileName( it->text(6)+"/"+it->text(0) );
		current->open(QIODevice::WriteOnly);
		socket->write( fpacket, sizeof(fpacket) );
	}
}

void fileTransferThread::parseFileList( const QByteArray &fl ) {
	parsedSize=0;
	filesParsed=0;
	fc=0;
	allSize=0;
	currentFile=0;
	QTreeWidgetItem *item;
	QStringList finfo;
	QString path = settings->profileValue( QString("filetransfer/%1").arg(rndid) ).toString(), fname;
	memcpy(&fc, fl.leftRef(4).constData(), 4);
	for(quint32 i=0; i<fc; ++i) {
		memcpy(&fs, fl.mid(4+i*260, 4).data(), 4);
		fname=fl.mid(4+i*260+4, 256);
		finfo<<fname<<""<<""<<""<<""<<QString("%L1 kB").arg( (float)qRound( ( (float)fs/1024 )*10 )/10 )<<path<<QString("%1").arg(fs);
		item = new QTreeWidgetItem(finfo);
		item->setIcon(0, QIcon(":offline"));
//		item->setCheckState(0,Qt::Checked);
		fileMap<<item;
		emit addListItem(item);
		allSize+=fs;
		finfo.clear();
	}
	settings->removeProfileValue( QString("filetransfer/%1").arg(rndid) );
	char packet[8];
	quint32 tmp=FileListAck;
	memcpy(&packet, &tmp,4);
	tmp=0;
	memcpy(&packet[4], &tmp,4);
	socket->write(packet,sizeof(packet));
	emit updateFilesData(fc, allSize);
	requestFile();
}

void fileTransferThread::newConnection() {
	qDebug()<<"New connection!";
	if( server->hasPendingConnections() ) {
		socket = server->nextPendingConnection();
		connect( socket, SIGNAL( readyRead() ), this, SLOT( readyRead() ) );
		connect( socket, SIGNAL( error(QAbstractSocket::SocketError) ), this, SLOT( error(QAbstractSocket::SocketError) ) );
		connect( socket, SIGNAL( disconnected() ), this, SLOT( disconnected() ) );
	}
}

void fileTransferThread::resetFilesData() {
	fileMap.clear();
	fc=0;
	allSize=0;
	currentFile=1;
	
}

void fileTransferThread::send() {
	QDomDocument doc;
	QDomElement f=doc.createElement("f");
//send: <f t="%receiver%" n="%filename%" e="1" i="%rndid%" c="%file_count%" s="%filesize%" v="1"/>
	f.setAttribute("t", owner);
	f.setAttribute("i", rndid);
	f.setAttribute("c", fc);
	f.setAttribute("s", allSize);
	f.setAttribute("e", "1");
	f.setAttribute("v", "1");
	if(fc==1)
		f.setAttribute("n", fileMap.at(0)->text(0) );
	doc.appendChild(f);
	Tlen->write(doc);
	emit prepareTransfering();
}

void fileTransferThread::sendFile( const QByteArray &f ) {
	cfTime = 0;
	cfParsed=0;
	if(!t->isActive())
		t->start(1000);
	memcpy(&currentFile, f.leftRef(4).constData(), 4);
	QTreeWidgetItem *it = fileMap.at(currentFile);
	current->setFileName( it->text(6)+"/"+it->text(0) );
	current->open(QIODevice::ReadOnly);

	do {
		sendFileChunk( current->read(1024) );
	} while( !current->atEnd() );
	socket->flush();
	sendFileClose();
}

void fileTransferThread::sendFileChunk( const QByteArray &f ) {
	char head[8];
	quint32 tmp = FileData;
	memcpy(&head, &tmp, 4);
	tmp=8+f.size();
	memcpy(&head[4], &tmp, 4);
	socket->write(head, 8);
	tmp=0;
	memcpy(&head, &tmp, 4);
	memcpy(&head[4], &tmp, 4);
	socket->write(head, 8);
	socket->write(f);
	parsedSize+=f.size();
	cfParsed+=f.size();
	emit updateTransferData(fc, filesParsed, allSize, parsedSize, allTime, cfTime, fileMap.at(currentFile));
	socket->flush();
	socket->waitForBytesWritten(-1);
}

void fileTransferThread::sendFileClose() {
	qDebug()<<"Closing file";
	current->close();
	char packet[12];
	quint32 tmp = EndOfFile;
	memcpy(&packet, &tmp, 4);
	tmp = 4;
	++filesParsed;
	memcpy(&packet[4], &tmp, 4);
	memcpy(&packet[8], &currentFile, 4);
	socket->write(packet, 12);
	socket->flush();
	if(filesParsed==fc)
		t->stop();
	emit updateTransferData(fc, filesParsed, allSize, parsedSize, allTime, cfTime, fileMap.at(currentFile));
}
