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

#include <fcntl.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <netinet/in.h>

#include "kadu.h"
#include "config_dialog.h"
#include "debug.h"
#include "message_box.h"

DccFileDialog::DccFileDialog(DccSocket* dccsocket, TransferType type, QDialog* parent, const char* name)
	: QDialog (parent, name), dccsocket(dccsocket), type(type)
{
	kdebugf();
	vlayout1 = new QVBoxLayout(this);
	vlayout1->setAutoAdd(true);
	vlayout1->setMargin(5);
	setWFlags(Qt::WDestructiveClose);
	prevPercent = 0;
	dccFinished = false;
	kdebugf2();
}

DccFileDialog::~DccFileDialog()
{
	kdebugf();
	delete time;
	if (!dccFinished)
	{
		kdebugmf(KDEBUG_WARNING, "DCC transfer has not finished yet!\n");
		delete dccsocket;
	}
	kdebugf2();
}

void DccFileDialog::closeEvent(QCloseEvent* e)
{
	QDialog::closeEvent(e);
}

void DccFileDialog::printFileInfo(struct gg_dcc* dccsock)
{
	kdebugf();
	long long int percent;
 	long double fpercent;

	QString sender;

	if (type == TRANSFER_TYPE_GET)
		sender=tr("Sender: %1");
	else
		sender=tr("Receiver: %1");
	new QLabel(sender.arg(userlist.byUin(dccsock->peer_uin).altNick()), this);

	new QLabel(tr("Filename: %1").arg((char *)dccsock->file_info.filename), this);

	new QLabel(tr("File size: %1B").arg(QString::number(dccsock->file_info.size)), this);

	l_offset = new QLabel(tr("Speed: 0kB/s (not started)  "),this);

	p_progress = new QProgressBar(100, this);
	p_progress->setProgress(0);

	time = new QTime();
	time->start();

	prevOffset = dccsock->offset;
	fpercent = ((long double)dccsock->offset * 100.0) / (long double)dccsock->file_info.size;
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

void DccFileDialog::updateFileInfo(struct gg_dcc *dccsock)
{
	kdebugf();
	long long int percent;
 	long double fpercent;
	int diffOffset,diffTime;

	if ((diffTime = time->elapsed()) > 1000)
	{
		diffOffset = dccsock->offset - prevOffset;
		prevOffset = dccsock->offset;
		QString str=tr("Speed: %1kB/s ").arg(QString::number(diffOffset/1024));
		if (!diffOffset)
			str.append(tr("(stalled)"));
		l_offset->setText(str);
		time->restart();
	}
	fpercent = ((long double)dccsock->offset * 100.0) / (long double)dccsock->file_info.size;
	percent = (long long int) fpercent;
	if (percent > prevPercent)
	{
		p_progress->setProgress(percent);
		prevPercent = percent;
	}
	setCaption(tr("File transfered %1%").arg((int)percent));
	kdebugf2();
}

FileTransferManager::FileTransferManager()
{
	kdebugf();
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
	connect(dcc_manager, SIGNAL(callbackReceived(DccSocket*)),
		this, SLOT(callbackReceived(DccSocket*)));
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
	disconnect(dcc_manager, SIGNAL(callbackReceived(DccSocket*)),
		this, SLOT(callbackReceived(DccSocket*)));
	disconnect(dcc_manager, SIGNAL(setState(DccSocket*)),
		this, SLOT(setState(DccSocket*)));
// FIXME: Segfaultuje gdy wcze¶niej zamkniemy okno transferu rêcznie.
/*	for (QMap<DccSocket*, DccFileDialog*>::const_iterator i = FileDialogs.begin();
		i != FileDialogs.end(); i++)
	{
		if (i.data()->isVisible())
			i.data()->close();
		else
			delete i.data();
	}*/
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
			config_file.readEntry("Network", "LastUploadDirectory", "~/")
			+(char *)socket->ggDccStruct()->file_info.filename,
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
	if (config_file.readBoolEntry("Network", "AllowDCC"))
		if (dcc_manager->configDccIp().isIp4Addr())
		{
			struct gg_dcc *dcc_new;
			UserBox *activeUserBox=UserBox::getActiveUserBox();
			UserList users;
			if (activeUserBox==NULL)
			{
				kdebugf2();
				return;
			}
			users= activeUserBox->getSelectedUsers();
			if (users.count() != 1)
			{
				kdebugf2();
				return;
			}
			UserListElement user = (*users.begin());
			if (user.port() >= 10)
			{
				kdebugm(KDEBUG_INFO, "ip: %s, port: %d, uin: %d\n", user.ip().toString().local8Bit().data(), user.port(), user.uin());
				if ((dcc_new = gadu->dccSendFile(htonl(user.ip().ip4Addr()), user.port(),
					config_file.readNumEntry("General", "UIN"), user.uin())) != NULL)
				{
					DccSocket* dcc = new DccSocket(dcc_new);
					connect(dcc, SIGNAL(dccFinished(DccSocket*)), dcc_manager,
						SLOT(dccFinished(DccSocket*)));
					dcc->initializeNotifiers();
				}
			}
			else
			{
				kdebugm(KDEBUG_INFO, "user.port()<10, asking for connection (uin: %d)\n", user.uin());
				dcc_manager->startTimeout();
				Requests.insert(user.uin(), true);
				gadu->dccRequest(user.uin());
			}
		}
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
	socket->setState(FileDialogs.contains(socket) ? DCC_SOCKET_TRANSFER_ERROR : DCC_SOCKET_CONNECTION_BROKEN);
	kdebugf2();
}

void FileTransferManager::dccError(DccSocket* socket)
{
	kdebugf();
	socket->setState(FileDialogs.contains(socket) ? DCC_SOCKET_TRANSFER_ERROR : DCC_SOCKET_CONNECTION_BROKEN);
	kdebugf2();
}

void FileTransferManager::needFileInfo(DccSocket* socket)
{
	kdebugf();
	QString f = selectFile(socket);
	if (f == QString::null)
	{
		kdebugmf(KDEBUG_NETWORK|KDEBUG_INFO, "Abort transfer\n");
		socket->setState(DCC_SOCKET_TRANSFER_DISCARDED);
		return;
	}
	gadu->dccFillFileInfo(socket->ggDccStruct(), f);
	DccFileDialog* filedialog = new DccFileDialog(socket, DccFileDialog::TRANSFER_TYPE_SEND, NULL, "dcc_file_dialog");
	filedialog->printFileInfo(socket->ggDccStruct());
	FileDialogs.insert(socket, filedialog);
	kdebugf2();
}

void FileTransferManager::needFileAccept(DccSocket* socket)
{
	kdebugf();

	QString str, f;
	QFileInfo fi;

	char fsize[20];
	snprintf(fsize, sizeof(fsize), "%.1f", (float) socket->ggDccStruct()->file_info.size / 1024);

	str=tr("User %1 wants to send us a file %2\nof size %3kB. Accept transfer?")
		.arg(userlist.byUin(socket->ggDccStruct()->peer_uin).altNick())
		.arg((char *)socket->ggDccStruct()->file_info.filename)
		.arg(fsize);

	switch (QMessageBox::information(0, tr("Incoming transfer"), str, tr("Yes"), tr("No"),
		QString::null, 0, 1))
	{
		case 0: // Yes?
			kdebugmf(KDEBUG_INFO, "accepted\n");
			f = QFileDialog::getSaveFileName(
				config_file.readEntry("Network", "LastDownloadDirectory", "~/")
					+(char *)socket->ggDccStruct()->file_info.filename,
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
				str = QString(tr("File %1 already exists.")).arg(f);
				switch (QMessageBox::information(0, tr("save file"),
					str, tr("Overwrite"), tr("Resume"),
					tr("Cancel"), 0, 2))
				{
					case 0:
						kdebugmf(KDEBUG_INFO, "truncating file %s\n", f.latin1());

						if ((socket->ggDccStruct()->file_fd = open(f.latin1(), O_WRONLY | O_CREAT | O_TRUNC, 0600)) == -1)
						{
							MessageBox::wrn(tr("Could not open file"));
							socket->setState(DCC_SOCKET_COULDNT_OPEN_FILE);
							kdebugf2();
							return;
						}
						socket->ggDccStruct()->offset = 0;
						break;
					case 1:
						kdebugmf(KDEBUG_INFO, "appending to file %s\n", f.latin1());

						if ((socket->ggDccStruct()->file_fd = open(f.latin1(), O_WRONLY | O_APPEND, 0600)) == -1)
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
				kdebugmf(KDEBUG_INFO, "creating file %s\n", f.latin1());

				if ((socket->ggDccStruct()->file_fd = open(f.latin1(), O_WRONLY | O_CREAT, 0600)) == -1)
				{
					MessageBox::wrn(tr("Could not open file"));
					socket->setState(DCC_SOCKET_COULDNT_OPEN_FILE);
					kdebugf2();
					return;
				}
				socket->ggDccStruct()->offset = 0;
			}

			FileDialogs.insert(socket,
				new DccFileDialog(socket, DccFileDialog::TRANSFER_TYPE_GET, NULL, "dcc_file_dialog"));
			FileDialogs[socket]->printFileInfo(socket->ggDccStruct());
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
	if (FileDialogs.contains(socket) && FileDialogs[socket]->isVisible())
		FileDialogs[socket]->updateFileInfo(socket->ggDccStruct());
	kdebugf2();
}

void FileTransferManager::dccDone(DccSocket* socket)
{
	kdebugf();
	if (FileDialogs.contains(socket) && FileDialogs[socket]->isVisible())
		FileDialogs[socket]->updateFileInfo(socket->ggDccStruct());
	kdebugf2();
}

void FileTransferManager::callbackReceived(DccSocket* socket)
{
	kdebugf();
	if (Requests.contains(socket->ggDccStruct()->peer_uin))
	{
		gadu->dccSetType(socket->ggDccStruct(), GG_SESSION_DCC_SEND);
		Requests.remove(socket->ggDccStruct()->peer_uin);
	}
	kdebugf2();
}

void FileTransferManager::setState(DccSocket* socket)
{
	kdebugf();
	if (FileDialogs.contains(socket))
		FileDialogs[socket]->dccFinished = true;
	kdebugf2();
}

void FileTransferManager::socketDestroying(DccSocket* socket)
{
	kdebugf();
	if (FileDialogs.contains(socket))
	{
		if (FileDialogs[socket]->dccFinished)
		{
			if (FileDialogs[socket]->isVisible())
				FileDialogs[socket]->close();
			else
				delete FileDialogs[socket];
			FileDialogs.remove(socket);
		}
	}
	kdebugf2();
}

FileTransferManager* file_transfer_manager = NULL;
