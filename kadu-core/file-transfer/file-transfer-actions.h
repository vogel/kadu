/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef FILE_TRANSFER_ACTIONS_H
#define FILE_TRANSFER_ACTIONS_H

#include <QtGui/QAction>

class ActionDescription;
class ContactSet;

class FileTransferActions : public QObject
{
	Q_OBJECT

	ActionDescription *SendFileActionDescription;
	ActionDescription *FileTransferWindowActionDescription;

	QStringList selectFilesToSend();
	void selectFilesAndSend(ContactSet contacts);

private slots:
	void sendFileActionActivated(QAction *sender, bool toggled);
	void toggleFileTransferWindow(QAction *sender, bool toggled);

public:
	explicit FileTransferActions(QObject *parent = 0);
	virtual ~FileTransferActions();

};

#endif // FILE_TRANSFER_ACTIONS_H
