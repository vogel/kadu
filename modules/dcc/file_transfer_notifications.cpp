/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <qfiledialog.h>
#include <qfileinfo.h>
#include <qmessagebox.h>

#include <fcntl.h>

#include "config_dialog.h"
#include "debug.h"
#include "file_transfer.h"
#include "message_box.h"
#include "misc.h"

#include "file_transfer_notifications.h"

NewFileTransferNotification::NewFileTransferNotification(FileTransfer *ft, DccSocket *socket, const UserListElements &userListElements,
	FileTransfer::StartType startType)
	: Notification("FileTransfer/IncomingFile", "SendFile", userListElements), ft(ft), socket(socket), fileName("")
{
	if (startType == FileTransfer::StartRestore)
	{
		addCallback("Continue", SLOT(callbackAccept()));
		addCallback("Save file under new name", SLOT(callbackAcceptAsNew()));
		addCallback("Ignore transfer", SLOT(callbackDiscard()));

		Continue = true;
	}
	else
	{
		addCallback("Accept", SLOT(callbackAccept()));
		addCallback("Discard", SLOT(callbackDiscard()));

		Continue = false;
	}

	setDefaultCallback(30 * 60 * 1000, SLOT(callbackDiscard()));
}

void NewFileTransferNotification::callbackAccept()
{
	kdebugf();

	if (Continue)
		file_transfer_manager->acceptFile(ft, socket, ft->fileName(), true);
	else
		file_transfer_manager->acceptFile(ft, socket, QString::null);
	close();
}

void NewFileTransferNotification::callbackAcceptAsNew()
{
	kdebugf();

	file_transfer_manager->acceptFile(ft, socket, QString::null);
	close();
}

void NewFileTransferNotification::callbackDiscard()
{
	kdebugf();

	file_transfer_manager->discardFile(socket);
	close();
}
