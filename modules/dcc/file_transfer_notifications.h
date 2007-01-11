/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef FILE_TRANSFER_NOTIFICATION_H
#define FILE_TRANSFER_NOTIFICATION_H

#include "../notify/notification.h"

#include "file_transfer.h"

class FileTransfer;
class FileTransferManager;
class DccSocket;

class NewFileTransferNotification : public Notification
{
	Q_OBJECT

private:

	bool Continue;

	FileTransfer *ft;
	DccSocket *socket;

	QString fileName;

private slots:

	void callbackAcceptAsNew();


public slots:

	virtual void callbackAccept();
	virtual void callbackDiscard();

public:

	NewFileTransferNotification(FileTransfer *ft, DccSocket *socket, const UserListElements &userListElements, FileTransfer::StartType startType);

};

#endif // FILE_TRANSFER_NOTIFICATION_H
