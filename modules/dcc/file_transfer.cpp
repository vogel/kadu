/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "file_transfer.h"

#include <qfiledialog.h>
#include <qmessagebox.h>
#include <qpushbutton.h>
#include <qfileinfo.h>
#include <qlayout.h>
#include <qsocketnotifier.h>
#include <qprogressbar.h>
#include <qlabel.h>

#include <fcntl.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <netinet/in.h>
#include <stdlib.h>

#include "kadu.h"
#include "config_dialog.h"
#include "debug.h"
#include "message_box.h"

FileTransferDialog::FileTransferDialog(DccSocket* socket, TransferType type)
	: QDialog (NULL, "file_transfer_dialog"), Socket(socket), Type(type)
{
	kdebugf();
	vlayout1 = new QVBoxLayout(this);
	vlayout1->setAutoAdd(true);
	vlayout1->setMargin(5);
	setWFlags(Qt::WDestructiveClose);
	prevPercent = 0;
	dccFinished = false;
	Dialogs.insert(socket, this);
	kdebugf2();
}

FileTransferDialog::~FileTransferDialog()
{
	kdebugf();
	Dialogs.remove(Socket);
	delete time;
	if (!dccFinished)
	{
		kdebugmf(KDEBUG_WARNING, "DCC transfer has not finished yet!\n");
		delete Socket;
	}
	kdebugf2();
}

void FileTransferDialog::printFileInfo()
{
	kdebugf();
	long long int percent;
 	long double fpercent;

	QString sender;

	if (Type == TRANSFER_TYPE_GET)
		sender=tr("Sender: %1");
	else
		sender=tr("Receiver: %1");
	new QLabel(sender.arg(userlist.byUin(Socket->ggDccStruct()->peer_uin).altNick()), this);

	new QLabel(tr("Filename: %1").arg(cp2unicode(Socket->ggDccStruct()->file_info.filename)), this);

	new QLabel(tr("File size: %1B").arg(QString::number(Socket->ggDccStruct()->file_info.size)), this);

	l_offset = new QLabel(tr("Speed: 0kB/s (not started)  "),this);

	p_progress = new QProgressBar(100, this);
	p_progress->setProgress(0);

	time = new QTime();
	time->start();

	prevOffset = Socket->ggDccStruct()->offset;
	fpercent = ((long double)Socket->ggDccStruct()->offset * 100.0) / (long double)Socket->ggDccStruct()->file_info.size;
	percent = (long long int) fpercent;
	if (percent > prevPercent)
	{
		p_progress->setProgress(percent);
		prevPercent = percent;
	}
	else
		p_progress->setProgress(0);

	resize(vlayout1->sizeHint());
	setMinimumSize(vlayout1->sizeHint());
	setFixedHeight(vlayout1->sizeHint().height());

	setCaption(tr("File transfered %1%").arg((int)percent));
	show();
	kdebugf2();
}

void FileTransferDialog::updateFileInfo()
{
	kdebugf();
	long long int percent;
 	long double fpercent;
	int diffOffset,diffTime;

	if ((diffTime = time->elapsed()) > 1000)
	{
		diffOffset = Socket->ggDccStruct()->offset - prevOffset;
		prevOffset = Socket->ggDccStruct()->offset;
		QString str=tr("Speed: %1kB/s ").arg(QString::number(diffOffset/1024));
		if (!diffOffset)
			str.append(tr("(stalled)"));
		l_offset->setText(str);
		time->restart();
	}
	fpercent = ((long double)Socket->ggDccStruct()->offset * 100.0) / (long double)Socket->ggDccStruct()->file_info.size;
	percent = (long long int) fpercent;
	if (percent > prevPercent)
	{
		p_progress->setProgress(percent);
		prevPercent = percent;
	}
	setCaption(tr("File transfered %1%").arg((int)percent));
	kdebugf2();
}

FileTransferDialog* FileTransferDialog::bySocket(DccSocket* socket)
{
	if (Dialogs.contains(socket))
		return Dialogs[socket];
	else
		return NULL;
}

void FileTransferDialog::destroyAll()
{
	kdebugf();
	while (!Dialogs.empty())
		delete Dialogs.begin().data();
	kdebugf2();
}

QMap<DccSocket*, FileTransferDialog*> FileTransferDialog::Dialogs;

FileTransferManager::FileTransferManager(QObject *parent, const char *name) : QObject(parent, name)
{
	kdebugf();
	config_file.addVariable("Network", "LastUploadDirectory", QString(getenv("HOME"))+"/");
	config_file.addVariable("Network", "LastDownloadDirectory", QString(getenv("HOME"))+"/");

	UserBox::userboxmenu->addItemAtPos(1, "SendFile", tr("Send file"),
		this,SLOT(sendFile()),
		HotKey::shortCutFromFile("ShortCuts", "kadu_sendfile"));
	connect(UserBox::userboxmenu,SIGNAL(popup()),this,SLOT(userboxMenuPopup()));
	connect(kadu, SIGNAL(keyPressed(QKeyEvent*)), this, SLOT(kaduKeyPressed(QKeyEvent*)));
	connect(dcc_manager, SIGNAL(connectionBroken(DccSocket*)),
		this, SLOT(connectionBroken(DccSocket*)));
	connect(dcc_manager, SIGNAL(dccError(DccSocket*)),
		this, SLOT(dccError(DccSocket*)));
	connect(dcc_manager, SIGNAL(needFileAccept(DccSocket*)),
		this, SLOT(needFileAccept(DccSocket*)));
	connect(dcc_manager, SIGNAL(needFileInfo(DccSocket*)),
		this, SLOT(needFileInfo(DccSocket*)));
	connect(dcc_manager, SIGNAL(noneEvent(DccSocket*)),
		this, SLOT(noneEvent(DccSocket*)));
	connect(dcc_manager, SIGNAL(dccDone(DccSocket*)),
		this, SLOT(dccDone(DccSocket*)));
	connect(dcc_manager, SIGNAL(setState(DccSocket*)),
		this, SLOT(setState(DccSocket*)));
	connect(dcc_manager, SIGNAL(socketDestroying(DccSocket*)),
		this, SLOT(socketDestroying(DccSocket*)));
	kdebugf2();
}

FileTransferManager::~FileTransferManager()
{
	kdebugf();
	int sendfile = UserBox::userboxmenu->getItem(tr("Send file"));
	UserBox::userboxmenu->removeItem(sendfile);
	disconnect(UserBox::userboxmenu,SIGNAL(popup()),this,SLOT(userboxMenuPopup()));
	disconnect(kadu, SIGNAL(keyPressed(QKeyEvent*)), this, SLOT(kaduKeyPressed(QKeyEvent*)));
	disconnect(dcc_manager, SIGNAL(connectionBroken(DccSocket*)),
		this, SLOT(connectionBroken(DccSocket*)));
	disconnect(dcc_manager, SIGNAL(dccError(DccSocket*)),
		this, SLOT(dccError(DccSocket*)));
	disconnect(dcc_manager, SIGNAL(needFileAccept(DccSocket*)),
		this, SLOT(needFileAccept(DccSocket*)));
	disconnect(dcc_manager, SIGNAL(needFileInfo(DccSocket*)),
		this, SLOT(needFileInfo(DccSocket*)));
	disconnect(dcc_manager, SIGNAL(noneEvent(DccSocket*)),
		this, SLOT(noneEvent(DccSocket*)));
	disconnect(dcc_manager, SIGNAL(dccDone(DccSocket*)),
		this, SLOT(dccDone(DccSocket*)));
	disconnect(dcc_manager, SIGNAL(setState(DccSocket*)),
		this, SLOT(setState(DccSocket*)));
	FileTransferDialog::destroyAll();
	kdebugf2();
}

void FileTransferManager::sendFile(UinType receiver, const QString &filename)
{
	kdebugf();
	if (config_file.readBoolEntry("Network", "AllowDCC") && dcc_manager->dccEnabled())
	{
		pendingFiles[receiver].push_back(filename);
		sendFile(receiver);
	}
	kdebugf2();
}

void FileTransferManager::sendFile(UinType receiver)
{
	kdebugf();
	if (config_file.readBoolEntry("Network", "AllowDCC"))
		if (dcc_manager->dccEnabled())
		{
			const UserListElement& user = userlist.byUin(receiver);
			DccManager::TryType type=dcc_manager->initDCCConnection(user.ip().ip4Addr(),
				user.port(),
				config_file.readNumEntry("General", "UIN"),
				user.uin(),
				SLOT(dccSendFile(uint32_t, uint16_t, UinType, UinType, struct gg_dcc **)),
				GG_SESSION_DCC_SEND);
			if (type==DccManager::DIRECT)
				direct.push_front(receiver);
		}
	kdebugf2();
}

QString FileTransferManager::selectFile(DccSocket* socket)
{
	kdebugf();
	QString f;
	QFileInfo fi;
	do
	{
		f = QFileDialog::getOpenFileName(
			config_file.readEntry("Network", "LastUploadDirectory")
			+cp2unicode(socket->ggDccStruct()->file_info.filename),
			QString::null, 0, tr("open file"), tr("Select file location"));
		fi.setFile(f);
		if (f!=QString::null && !fi.isReadable())
			MessageBox::msg(tr("This file is not readable"), true);
	}
	while (f != QString::null && !fi.isReadable());
	if (f!=QString::null && fi.isReadable())
		config_file.writeEntry("Network", "LastUploadDirectory", fi.dirPath()+"/");
	kdebugf2();
	return f;
}

void FileTransferManager::sendFile()
{
	kdebugf();

	UserBox *activeUserBox=UserBox::getActiveUserBox();
	UserList users;
	if (activeUserBox==NULL)
	{
		kdebugf2();
		return;
	}
	users = activeUserBox->getSelectedUsers();
	if (users.count() != 1)
	{
		kdebugf2();
		return;
	}
	UserListElement user = (*users.begin());
	sendFile(user.uin());
	
	kdebugf2();
}

void FileTransferManager::userboxMenuPopup()
{
	kdebugf();
	int sendfile = UserBox::userboxmenu->getItem(tr("Send file"));
	UserBox* activeUserBox=UserBox::getActiveUserBox();
	if (activeUserBox==NULL)//to siê zdarza...
	{
		kdebugf2();
		return;
	}
	UserList users = activeUserBox->getSelectedUsers();
	UserListElement user = (*users.begin());

	bool containsOurUin=users.containsUin(config_file.readNumEntry("General", "UIN"));
	bool userIsOnline = user.status().isOnline() || user.status().isBusy();
	bool dccEnabled = (users.count() == 1 &&
		config_file.readBoolEntry("Network", "AllowDCC") &&
		!containsOurUin &&
		userIsOnline &&
		DccSocket::count() < 8);

	UserBox::userboxmenu->setItemEnabled(sendfile, dccEnabled);
	kdebugf2();
}

void FileTransferManager::kaduKeyPressed(QKeyEvent* e)
{
	if (HotKey::shortCut(e,"ShortCuts", "kadu_sendfile"))
		sendFile();
}

void FileTransferManager::connectionBroken(DccSocket* socket)
{
	kdebugf();
	if (FileTransferDialog::bySocket(socket) != NULL)
		socket->setState(DCC_SOCKET_TRANSFER_ERROR);
	else
		kdebugm(KDEBUG_INFO, "not my socket\n");
	kdebugf2();
}

void FileTransferManager::dccError(DccSocket* socket)
{
	kdebugf();
	if (FileTransferDialog::bySocket(socket) != NULL)
	{
		socket->setState(DCC_SOCKET_TRANSFER_ERROR);
		UinType peer_uin=socket->ggDccStruct()->peer_uin;
		if (direct.contains(peer_uin))
		{
			direct.remove(peer_uin);
			const UserListElement& user = userlist.byUin(peer_uin);
			dcc_manager->initDCCConnection(user.ip().ip4Addr(),
					user.port(),
					config_file.readNumEntry("General", "UIN"),
					user.uin(),
					SLOT(dccSendFile(uint32_t, uint16_t, UinType, UinType, struct gg_dcc **)),
					GG_SESSION_DCC_SEND, true);
		}
	}
	else
		kdebugm(KDEBUG_INFO, "not my socket\n");
	kdebugf2();
}

void FileTransferManager::needFileInfo(DccSocket* socket)
{
	kdebugf();
	//je¿eli druga strona prosi o plik, to znaczy,
	//¿e nie bêdziemy potrzebowali po³±czenia zwrotnego
	UinType peer_uin=socket->ggDccStruct()->peer_uin;
	if (direct.contains(peer_uin))
		direct.remove(peer_uin);

	QString filename;
	if (pendingFiles.contains(peer_uin))
	{
		filename = pendingFiles[peer_uin].front();
		pendingFiles[peer_uin].pop_front();
		if (pendingFiles[peer_uin].empty())
			pendingFiles.remove(peer_uin);
	}
	else
		filename = selectFile(socket);

	if (filename.isEmpty())
	{
		kdebugmf(KDEBUG_NETWORK|KDEBUG_INFO, "Abort transfer\n");
		socket->setState(DCC_SOCKET_TRANSFER_DISCARDED);
		return;
	}
	
	gadu->dccFillFileInfo(socket->ggDccStruct(), filename);
	FileTransferDialog* filedialog = new FileTransferDialog(socket, FileTransferDialog::TRANSFER_TYPE_SEND);
	filedialog->printFileInfo();

	kdebugf2();
}

void FileTransferManager::needFileAccept(DccSocket* socket)
{
	kdebugf();

	QString str, f;
	QFileInfo fi;

	char fsize[20];
	snprintf(fsize, sizeof(fsize), "%.1f", (float) socket->ggDccStruct()->file_info.size / 1024);

	str=narg(tr("User %1 wants to send us a file %2\nof size %3kB. Accept transfer?"),
		userlist.byUin(socket->ggDccStruct()->peer_uin).altNick(),
		cp2unicode(socket->ggDccStruct()->file_info.filename),
		QString(fsize));

	switch (QMessageBox::information(0, tr("Incoming transfer"), str, tr("Yes"), tr("No"),
		QString::null, 0, 1))
	{
		case 0: // Yes?
			kdebugmf(KDEBUG_INFO, "accepted\n");
			f = QFileDialog::getSaveFileName(
				config_file.readEntry("Network", "LastDownloadDirectory")
					+cp2unicode(socket->ggDccStruct()->file_info.filename),
				QString::null, 0, tr("save file"), tr("Select file location"));
			if (f.isEmpty())
			{
				kdebugmf(KDEBUG_INFO, "discarded\n");
				socket->setState(DCC_SOCKET_TRANSFER_DISCARDED);
				return;
			}
			config_file.writeEntry("Network", "LastDownloadDirectory", QFileInfo(f).dirPath()+"/");
			fi.setFile(f);
			if (fi.exists() && fi.size() < socket->ggDccStruct()->file_info.size)
			{
				str.truncate(0);
				str = tr("File %1 already exists.").arg(f);
				switch (QMessageBox::information(0, tr("save file"),
					str, tr("Overwrite"), tr("Resume"),
					tr("Cancel"), 0, 2))
				{
					case 0:
						kdebugmf(KDEBUG_INFO, "truncating file %s\n", f.local8Bit().data());

						if ((socket->ggDccStruct()->file_fd = open(f.local8Bit().data(), O_WRONLY | O_CREAT | O_TRUNC, 0600)) == -1)
						{
							MessageBox::wrn(tr("Could not open file"));
							socket->setState(DCC_SOCKET_COULDNT_OPEN_FILE);
							kdebugf2();
							return;
						}
						socket->ggDccStruct()->offset = 0;
						break;
					case 1:
						kdebugmf(KDEBUG_INFO, "appending to file %s\n", f.local8Bit().data());

						if ((socket->ggDccStruct()->file_fd = open(f.local8Bit().data(), O_WRONLY | O_APPEND, 0600)) == -1)
						{
							MessageBox::wrn(tr("Could not open file"));
							socket->setState(DCC_SOCKET_COULDNT_OPEN_FILE);
							kdebugf2();
							return;
						}
						socket->ggDccStruct()->offset = fi.size();
						break;
					case 2:
						kdebugmf(KDEBUG_INFO, "discarded\n");
						socket->setState(DCC_SOCKET_TRANSFER_DISCARDED);
						kdebugf2();
						return;
				}
			}
			else
			{
				kdebugmf(KDEBUG_INFO, "creating file %s\n", f.local8Bit().data());

				if ((socket->ggDccStruct()->file_fd = open(f.local8Bit().data(), O_WRONLY | O_CREAT, 0600)) == -1)
				{
					MessageBox::wrn(tr("Could not open file"));
					socket->setState(DCC_SOCKET_COULDNT_OPEN_FILE);
					kdebugf2();
					return;
				}
				socket->ggDccStruct()->offset = 0;
			}

			new FileTransferDialog(socket, FileTransferDialog::TRANSFER_TYPE_GET);
			FileTransferDialog::bySocket(socket)->printFileInfo();
			break;
		case 1:
			kdebugmf(KDEBUG_INFO, "discarded\n");
			socket->setState(DCC_SOCKET_TRANSFER_DISCARDED);
			break;
	}
	kdebugf2();
}

void FileTransferManager::noneEvent(DccSocket* socket)
{
	kdebugf();

	FileTransferDialog *dialog=FileTransferDialog::bySocket(socket);
	if (dialog != NULL)
		dialog->updateFileInfo();
	else
		kdebugm(KDEBUG_INFO, "not my socket\n");

	kdebugf2();
}

void FileTransferManager::dccDone(DccSocket* socket)
{
	kdebugf();

	FileTransferDialog *dialog=FileTransferDialog::bySocket(socket);
	if (dialog != NULL)
		dialog->updateFileInfo();
	else
		kdebugm(KDEBUG_INFO, "not my socket\n");

	kdebugf2();
}

void FileTransferManager::setState(DccSocket* socket)
{
	kdebugf();

	FileTransferDialog *dialog=FileTransferDialog::bySocket(socket);
	if (dialog != NULL)
		dialog->dccFinished = true;
	else
		kdebugm(KDEBUG_INFO, "not my socket\n");

	kdebugf2();
}

void FileTransferManager::socketDestroying(DccSocket* socket)
{
	kdebugf();

	FileTransferDialog *dialog=FileTransferDialog::bySocket(socket);
	if (dialog != NULL)
	{
		UinType peer_uin=socket->ggDccStruct()->peer_uin;
		if (direct.contains(peer_uin))
			direct.remove(peer_uin);
		delete dialog;
	}
	else
		kdebugm(KDEBUG_INFO, "not my socket\n");

	kdebugf2();
}

FileTransferManager* file_transfer_manager = NULL;
