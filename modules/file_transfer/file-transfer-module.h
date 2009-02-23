/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef FILE_TRANSFER_MODULE
#define FILE_TRANSFER_MODULE

#include <QtCore/QObject>

class QAction;
class QStringList;

class ActionDescription;
class ContactList;
class FileTransferWindow;

class FileTransferModule : public QObject
{
	Q_OBJECT

	static FileTransferModule Module;

	ActionDescription *SendFileActionDescription;
	ActionDescription *FileTransferWindowActionDescription;

	FileTransferWindow *Window;

	FileTransferModule();
	~FileTransferModule();

	void createActionDecriptions();
	void deleteActionDecriptions();

	QStringList selectFilesToSend();
	void selectFilesAndSend(ContactList contacts);

private slots:
	void sendFileActionActivated(QAction *sender, bool toggled);
	void toggleFileTransferWindow(QAction *sender, bool toggled);
	void showFileTransferWindow();

	void fileTransferWindowDestroyed();

};

#endif //  FILE_TRANSFER_MODULE
