/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <fcntl.h> 

#include <qmessagebox.h>

#include "chat_manager.h"
#include "config_file.h"
#include "dcc.h"
#include "debug.h"
#include "file_transfer.h"
#include "file_transfer_notifications.h"
#include "file_transfer_window.h"
#include "icons_manager.h"
#include "kadu.h"
#include "message_box.h"
#include "protocol.h"

#include "../notify/notify.h"

#include "file_transfer_manager.h"

FileTransferManager::FileTransferManager(QObject *parent, const char *name) : QObject(parent, name),
	fileTransferWindow(0), toggleFileTransferWindowMenuId(0)
{
	kdebugf();

	UserBox::userboxmenu->addItemAtPos(1, "SendFile", tr("Send file"),
		this, SLOT(sendFile()),
		HotKey::shortCutFromFile("ShortCuts", "kadu_sendfile"));
	connect(UserBox::userboxmenu,SIGNAL(popup()), this, SLOT(userboxMenuPopup()));
	connect(kadu, SIGNAL(keyPressed(QKeyEvent*)), this, SLOT(kaduKeyPressed(QKeyEvent*)));

	Action* send_file_action = new Action("SendFile", tr("Send file"), "sendFileAction", Action::TypeUser);
	connect(send_file_action, SIGNAL(activated(const UserGroup*, const QWidget*, bool)),
		this, SLOT(sendFileActionActivated(const UserGroup*)));

	connect(chat_manager, SIGNAL(chatWidgetCreated(ChatWidget *)), this, SLOT(chatCreated(ChatWidget *)));
	connect(chat_manager, SIGNAL(chatWidgetDestroying(ChatWidget *)), this, SLOT(chatDestroying(ChatWidget*)));

	FOREACH(it, chat_manager->chats())
		chatCreated(*it);

	connect(dcc_manager, SIGNAL(connectionBroken(DccSocket*)),
		this, SLOT(connectionBroken(DccSocket*)));
	connect(dcc_manager, SIGNAL(dccEvent(DccSocket*, bool&)),
		this, SLOT(dccEvent(DccSocket*, bool&)));
	connect(dcc_manager, SIGNAL(dccError(DccSocket*)),
		this, SLOT(dccError(DccSocket*)));
	connect(dcc_manager, SIGNAL(setState(DccSocket*)),
		this, SLOT(setState(DccSocket*)));
	connect(dcc_manager, SIGNAL(socketDestroying(DccSocket*)),
		this, SLOT(socketDestroying(DccSocket*)));

	QPopupMenu *MainMenu = kadu->mainMenu();
	toggleFileTransferWindowMenuId = MainMenu->insertItem(icons_manager->loadIcon("SendFileWindow"), tr("Toggle transfers window"),
		this, SLOT(toggleFileTransferWindow()), 0, -1, 10);
	icons_manager->registerMenuItem(MainMenu, tr("Toggle transfers window"), "SendFileWindow");

	notification_manager->registerEvent("FileTransfer/IncomingFile",  QT_TRANSLATE_NOOP("@default", "An user wants to send you a file"), CallbackRequired);
	notification_manager->registerEvent("FileTransfer/Finished", QT_TRANSLATE_NOOP("@default", "File transfer was finished"), CallbackNotRequired);

	readFromConfig();

	kdebugf2();
}

FileTransferManager::~FileTransferManager()
{
	kdebugf();

	writeToConfig();

 	notification_manager->unregisterEvent("FileTransfer/IncomingFile");
	notification_manager->unregisterEvent("FileTransfer/Finished");

	int sendfile = UserBox::userboxmenu->getItem(tr("Send file"));
	UserBox::userboxmenu->removeItem(sendfile);
	disconnect(UserBox::userboxmenu,SIGNAL(popup()), this, SLOT(userboxMenuPopup()));
	disconnect(kadu, SIGNAL(keyPressed(QKeyEvent*)), this, SLOT(kaduKeyPressed(QKeyEvent*)));

	Action *sendFileAction = KaduActions["sendFileAction"];
	delete sendFileAction;

	disconnect(chat_manager, SIGNAL(chatWidgetCreated(ChatWidget *)), this, SLOT(chatCreated(ChatWidget *)));
	disconnect(chat_manager, SIGNAL(chatWidgetDestroying(ChatWidget *)), this, SLOT(chatDestroying(ChatWidget *)));

	FOREACH(it, chat_manager->chats())
		chatDestroying(*it);

	disconnect(dcc_manager, SIGNAL(connectionBroken(DccSocket*)),
		this, SLOT(connectionBroken(DccSocket*)));
	disconnect(dcc_manager, SIGNAL(dccEvent(DccSocket*,bool&)),
		this, SLOT(dccEvent(DccSocket*,bool&)));
	disconnect(dcc_manager, SIGNAL(dccError(DccSocket*)),
		this, SLOT(dccError(DccSocket*)));
	disconnect(dcc_manager, SIGNAL(setState(DccSocket*)),
		this, SLOT(setState(DccSocket*)));

	FileTransfer::destroyAll();

	kadu->mainMenu()->removeItem(toggleFileTransferWindowMenuId);

	if (fileTransferWindow)
	{
		disconnect(this, SIGNAL(newFileTransfer(FileTransfer *)),
			fileTransferWindow, SLOT(newFileTransfer(FileTransfer *)));
		delete fileTransferWindow;
	}

	kdebugf2();
}

void FileTransferManager::readFromConfig()
{
	kdebugf();

	FileTransfer::destroyAll();
	QDomElement fts_elem = xml_config_file->findElement(xml_config_file->rootElement(), "FileTransfers");
	if (fts_elem.isNull())
		return;

	QDomNodeList ft_list = fts_elem.elementsByTagName("FileTransfer");
	FileTransfer *ft;
	for (unsigned int i = 0; i < ft_list.count(); i++)
	{
		ft = FileTransfer::fromDomElement(ft_list.item(i).toElement(), this);
		connect(ft, SIGNAL(fileTransferFinished(FileTransfer *, bool)), this, SLOT(fileTransferFinishedSlot(FileTransfer *, bool)));
	}

	kdebugf2();
}

void FileTransferManager::writeToConfig()
{
	kdebugf();

	QDomElement root_elem = xml_config_file->rootElement();
	QDomElement fts_elem = xml_config_file->accessElement(root_elem, "FileTransfers");
	xml_config_file->removeChildren(fts_elem);
	CONST_FOREACH(i, FileTransfer::AllTransfers)
		(*i)->toDomElement(fts_elem);
	xml_config_file->sync();

	kdebugf2();
}

QStringList FileTransferManager::selectFilesToSend()
{
	return QFileDialog::getOpenFileNames(
		QString::null,
		config_file.readEntry("Network", "LastUploadDirectory"),
		0, "open file", tr("Select file location"));
}

void FileTransferManager::sendFile(UinType receiver, const QString &filename)
{
	kdebugf();

	FileTransfer *ft = FileTransfer::search(FileTransfer::TypeSend, receiver, filename);
	if (!ft)
		ft = new FileTransfer(this, FileTransfer::TypeSend, receiver, filename);

	if (!fileTransferWindow)
		toggleFileTransferWindow();

	ft->start();

	kdebugf2();
}

void FileTransferManager::sendFile(UinType receiver)
{
	kdebugf();

	QStringList files = selectFilesToSend();
	if (!files.count())
	{
		kdebugf2();
		return;
	}

	CONST_FOREACH(file, files)
		sendFile(receiver, *file);

	kdebugf2();
}

void FileTransferManager::sendFile()
{
	kdebugf();

	UserBox *activeUserBox = UserBox::activeUserBox();
	if (activeUserBox)
		sendFile(activeUserBox->selectedUsers());

	kdebugf2();
}

void FileTransferManager::sendFileActionActivated(const UserGroup* users)
{
	kdebugf();

	if (users->count())
		sendFile(users->toUserListElements());

	kdebugf2();
}

void FileTransferManager::sendFile(const UserListElements users)
{
	kdebugf();

	QStringList files = selectFilesToSend();
	if (!files.count())
	{
		kdebugf2();
		return;
	}

	unsigned int myUin = config_file.readUnsignedNumEntry("General", "UIN");

	CONST_FOREACH(user, users)
		CONST_FOREACH(file, files)
			if ((*user).usesProtocol("Gadu") && (*user).ID("Gadu") != myUin)
				sendFile((*user).ID("Gadu").toUInt(), *file);

	kdebugf2();
}

void FileTransferManager::userboxMenuPopup()
{
	kdebugf();

	UserBox *activeUserBox = UserBox::activeUserBox();
	if (activeUserBox == NULL)
	{
		kdebugf2();
		return;
	}

	int sendfile = UserBox::userboxmenu->getItem(tr("Send file"));
	bool dccEnabled = config_file.readBoolEntry("Network", "AllowDCC");
	bool dccKeyEnabled = true;

	if (dccEnabled)
	{
		unsigned int myUin = config_file.readUnsignedNumEntry("General", "UIN");
		UserListElements users = activeUserBox->selectedUsers();

		CONST_FOREACH(user, users)
			if (!(*user).usesProtocol("Gadu") || (*user).ID("Gadu").toUInt() == myUin)
			{
				dccKeyEnabled = false;
				break;
			}
	}

	UserBox::userboxmenu->setItemVisible(sendfile, dccKeyEnabled && dccEnabled);
	kdebugf2();
}

void FileTransferManager::kaduKeyPressed(QKeyEvent* e)
{
	if (HotKey::shortCut(e,"ShortCuts", "kadu_sendfile"))
		sendFile();
}

void FileTransferManager::chatCreated(ChatWidget *chat)
{
	connect(chat, SIGNAL(fileDropped(const UserGroup *, const QString &)),
		this, SLOT(fileDropped(const UserGroup *, const QString &)));
}

void FileTransferManager::chatDestroying(ChatWidget *chat)
{
	disconnect(chat, SIGNAL(fileDropped(const UserGroup *, const QString &)),
		this, SLOT(fileDropped(const UserGroup *, const QString &)));
}

void FileTransferManager::fileDropped(const UserGroup *group, const QString &fileName)
{
	CONST_FOREACH(i, *group)
		if ((*i).usesProtocol("Gadu"))
			sendFile((*i).ID("Gadu").toUInt(), fileName);
}

void FileTransferManager::showFileTransferWindow()
{
	if (!fileTransferWindow)
		toggleFileTransferWindow();
}

void FileTransferManager::toggleFileTransferWindow()
{
	kdebugmf(KDEBUG_FUNCTION_START, "start: fileTransferWindow:%p\n", fileTransferWindow);
	if (fileTransferWindow)
	{
		disconnect(this, SIGNAL(newFileTransfer(FileTransfer *)),
			fileTransferWindow, SLOT(newFileTransfer(FileTransfer *)));
		disconnect(this, SIGNAL(fileTransferDestroying(FileTransfer *)),
			fileTransferWindow, SLOT(fileTransferDestroying(FileTransfer *)));
		disconnect(fileTransferWindow, SIGNAL(destroyed()), this, SLOT(fileTransferWindowDestroyed()));
		delete fileTransferWindow;
		fileTransferWindow = 0;
	}
	else
	{
		fileTransferWindow = new FileTransferWindow();
		connect(fileTransferWindow, SIGNAL(destroyed()), this, SLOT(fileTransferWindowDestroyed()));
		connect(this, SIGNAL(newFileTransfer(FileTransfer *)),
			fileTransferWindow, SLOT(newFileTransfer(FileTransfer *)));
		connect(this, SIGNAL(fileTransferDestroying(FileTransfer *)),
			fileTransferWindow, SLOT(fileTransferDestroying(FileTransfer *)));
		fileTransferWindow->show();
	}
	kdebugf2();
}

void FileTransferManager::fileTransferFinishedSlot(FileTransfer *fileTransfer, bool ok)
{
	QString message;

	if (ok && config_file.readBoolEntry("Network", "RemoveCompletedTransfers"))
		fileTransfer->deleteLater();

	if (ok)
		message = tr("File has been transferred sucessfully.");
	else
		message = tr("File transfer error!");

	Notification *fileTransferFinishedNotification = new Notification("FileTransfer/Finished", "SendFile", UserListElements());
	fileTransferFinishedNotification->setTitle(tr("File transfer finished"));
	fileTransferFinishedNotification->setText(message);

	notification_manager->notify(fileTransferFinishedNotification);
}

void FileTransferManager::fileTransferWindowDestroyed()
{
	kdebugf();
	fileTransferWindow = 0;
}

void FileTransferManager::connectionBroken(DccSocket* socket)
{
	kdebugf();
	FileTransfer *ft = FileTransfer::bySocket(socket);
	if (ft)
	{
		ft->connectionBroken();
		return;
	}
	else
		kdebugm(KDEBUG_INFO, "not my socket\n");
	kdebugf2();
}

void FileTransferManager::dccEvent(DccSocket *socket, bool &lock)
{
	kdebugf();

	switch (socket->ggDccEvent()->type)
	{
		case GG_EVENT_DCC_NEED_FILE_ACK:
			kdebugmf(KDEBUG_NETWORK|KDEBUG_INFO, "GG_EVENT_DCC_NEED_FILE_ACK! uin:%d peer_uin:%d\n",
				socket->ggDccStruct()->uin, socket->ggDccStruct()->peer_uin);
			needFileAccept(socket);
			lock = true;
			break;

		case GG_EVENT_DCC_NEED_FILE_INFO:
			kdebugmf(KDEBUG_NETWORK|KDEBUG_INFO, "GG_EVENT_DCC_NEED_FILE_INFO! uin:%d peer_uin:%d\n",
				socket->ggDccStruct()->uin, socket->ggDccStruct()->peer_uin);
			needFileInfo(socket);
			break;

		default:
			break;
	}

	kdebugf2();
}

void FileTransferManager::dccError(DccSocket* socket)
{
	kdebugf();
	FileTransfer *ft = FileTransfer::bySocket(socket);
	if (ft)
	{
		ft->dccError();
		return;
	}
	else
		kdebugm(KDEBUG_INFO, "not my socket\n");
	kdebugf2();
}

void FileTransferManager::needFileInfo(DccSocket* socket)
{
	kdebugf();

	FileTransfer *ft = FileTransfer::byUinAndStatus(socket->ggDccStruct()->peer_uin,
		FileTransfer::StatusWaitForConnection);
	if (ft)
	{
		ft->setSocket(socket);
		ft->needFileInfo();
		return;
	}

	kdebugf2();
}

void FileTransferManager::needFileAccept(DccSocket *socket)
{
	kdebugf();

	QString fileName;
	QString question;

	QString fileSize = QString("%1").arg((float)(socket->ggDccStruct()->file_info.size / 1024), 0, 'f', 2);

 	FileTransfer *ft = FileTransfer::search(FileTransfer::TypeReceive, socket->ggDccStruct()->peer_uin,
 		cp2unicode(socket->ggDccStruct()->file_info.filename), FileTransfer::FileNameGadu);

	NewFileTransferNotification *newFileTransferNotification;

 	if (ft)
 	{
		newFileTransferNotification = new NewFileTransferNotification(ft, socket,
			userlist->byID("Gadu", QString::number(socket->ggDccStruct()->peer_uin)), FileTransfer::StartRestore);

 		question = narg(tr("User %1 want to send you a file %2\nof size %3kB.\n"
 		                   "This is probably a next part of %4\n What should I do?"),
 			userlist->byID("Gadu", QString::number(socket->ggDccStruct()->peer_uin)).altNick(),
 			cp2unicode(socket->ggDccStruct()->file_info.filename),
			fileSize,
 			ft->fileName()
 		);
	}
	else
	{
		newFileTransferNotification = new NewFileTransferNotification(ft, socket,
			userlist->byID("Gadu", QString::number(socket->ggDccStruct()->peer_uin)), FileTransfer::StartNew);

 		question = narg(tr("User %1 wants to send us a file %2\nof size %3kB. Accept transfer?"),
 			userlist->byID("Gadu", QString::number(socket->ggDccStruct()->peer_uin)).altNick(),
 			cp2unicode(socket->ggDccStruct()->file_info.filename),
			fileSize
		);
	}

	newFileTransferNotification->setText(question);
	newFileTransferNotification->setTitle("Incoming transfer");

	notification_manager->notify(newFileTransferNotification);

	kdebugf2();
}

void FileTransferManager::acceptFile(FileTransfer *ft, DccSocket *socket, QString fileName, bool resume)
{
	kdebugf();

	bool haveFileName = !fileName.isNull();

	QFileInfo fi;

	while (true)
	{
		if (socket == NULL)
		{
			kdebugm(KDEBUG_INFO, "socket is null");
			return;
		}

		if (socket->ggDccStruct() == NULL)
		{
			kdebugm(KDEBUG_INFO, "socket ggDccStruct is null");
			socket->discard();
			return;
		}

		if (!haveFileName || fileName.isEmpty())
			fileName = QFileDialog::getSaveFileName(config_file.readEntry("Network", "LastDownloadDirectory")
				+ cp2unicode(socket->ggDccStruct()->file_info.filename),
				QString::null, 0, "save file", tr("Select file location"));

		if (fileName.isEmpty())
		{
			kdebugmf(KDEBUG_INFO, "discarded\n");
			socket->discard();
			return;
		}

		config_file.writeEntry("Network", "LastDownloadDirectory", QFileInfo(fileName).dirPath() + '/');
		fi.setFile(fileName);

		if (!haveFileName && fi.exists())
		{
			QString question;
			question = tr("File %1 already exists.").arg(fileName);

			switch (QMessageBox::question(0, tr("save file"), question, tr("Overwrite"), tr("Resume"),
			                                 tr("Select another file"), 0, 2))
			{
				case 0:
					resume = false;
					break;

				case 1:
					resume = true;
					break;

				case 2:
					continue;
			}
		}

		haveFileName = false;
		int flags = O_WRONLY;
		if (resume)
			flags |= O_APPEND;
		else
			flags |= O_CREAT | O_TRUNC;

		if ((socket->ggDccStruct()->file_fd = open(fileName.local8Bit().data(), flags, 0600)) == -1)
			MessageBox::msg(tr("Could not open file. Select another one."), false, "Warning");
		else
		{
			socket->ggDccStruct()->offset = fi.size();

 			FileTransfer *ft = FileTransfer::search(FileTransfer::TypeReceive, socket->ggDccStruct()->peer_uin,
		 		fileName);

			if (!ft)
				ft = new FileTransfer(this, FileTransfer::TypeReceive, socket->ggDccStruct()->peer_uin, fileName);

			ft->setSocket(socket);
			showFileTransferWindow();

			ft->start();
			socket->enableNotifiers();

			break;
		}
	}

	kdebugf2();
}

void FileTransferManager::discardFile(DccSocket *socket)
{
	kdebugf();
	socket->discard();
	kdebugf2();
}

void FileTransferManager::setState(DccSocket* socket)
{
	kdebugf();

	FileTransfer *ft = FileTransfer::bySocket(socket);
	if (ft != NULL)
		ft->finished(socket->state() == DCC_SOCKET_TRANSFER_FINISHED);
	else
		kdebugm(KDEBUG_INFO, "not my socket\n");

	kdebugf2();
}

void FileTransferManager::socketDestroying(DccSocket* socket)
{
	kdebugf();

	FileTransfer *ft = FileTransfer::bySocket(socket);
	if (ft)
		ft->socketDestroying();
	else
		kdebugm(KDEBUG_INFO, "not my socket\n");

	kdebugf2();
}

FileTransferManager* file_transfer_manager = NULL;
