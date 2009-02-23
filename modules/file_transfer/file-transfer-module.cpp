/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <stdio.h>

#include <QtGui/QFileDialog>

#include "accounts/account.h"
#include "contacts/contact.h"
#include "file-transfer/file-transfer.h"
#include "file-transfer/file-transfer-manager.h"
#include "protocols/protocol.h"
#include "protocols/services/file-transfer-service.h"
#include "gui/widgets/contacts-list-widget-menu-manager.h"

#include "action.h"
#include "config_file.h"
#include "debug.h"
#include "exports.h"
#include "kadu.h"

#include "gui/windows/file-transfer-window.h"

#include "file-transfer-module.h"

FileTransferModule FileTransferModule::Module;

extern "C" KADU_EXPORT int file_transfer_init(bool firstLoad)
{
	kdebugf();
//	MainConfigurationWindow::registerUiFile(dataPath("kadu/modules/configuration/dcc.ui"), dcc_manager);

	return 0;
}

extern "C" KADU_EXPORT void file_transfer_close()
{
	kdebugf();

//	MainConfigurationWindow::unregisterUiFile(dataPath("kadu/modules/configuration/dcc.ui"), dcc_manager);
}

void disableNonFileTransferContacts(KaduAction *action)
{
	kdebugf();

	action->setEnabled(false);

	const ContactList &contacts = action->contacts();

	if (!contacts.count())
		return;

	foreach (const Contact &contact, contacts)
	{
		if (kadu->myself() == contact)
			return;

		Account *account = contact.prefferedAccount();
		if (0 == account->protocol()->fileTransferService())
			return;
	}

	action->setEnabled(true);
}

FileTransferModule::FileTransferModule() :
		Window(0)
{
	createActionDecriptions();
}

FileTransferModule::~FileTransferModule()
{
	deleteActionDecriptions();

	if (Window)
	{
		delete Window;
		Window = 0;
	}
}

void FileTransferModule::createActionDecriptions()
{
	SendFileActionDescription = new ActionDescription(
		ActionDescription::TypeUser, "sendFileAction",
		this, SLOT(sendFileActionActivated(QAction *, bool)),
		"SendFile", tr("Send file"), false, QString::null,
		disableNonFileTransferContacts
	);
	SendFileActionDescription->setShortcut("kadu_sendfile");
	ContactsListWidgetMenuManager::instance()->insertActionDescription(1, SendFileActionDescription);

	FileTransferWindowActionDescription = new ActionDescription(
		ActionDescription::TypeMainMenu, "sendFileWindowAction",
		this, SLOT(toggleFileTransferWindow(QAction *, bool)),
		"SendFileWindow", tr("File transfers")
	);
	kadu->insertMenuActionDescription(FileTransferWindowActionDescription, Kadu::MenuKadu, 6); // TODO 0.6.6: update
}

void FileTransferModule::deleteActionDecriptions()
{
	ContactsListWidgetMenuManager::instance()->removeActionDescription(SendFileActionDescription);
// 	kadu->removeMenuActionDescription(FileTransferWindowActionDescription);

	delete SendFileActionDescription;
	SendFileActionDescription = 0;
	delete FileTransferWindowActionDescription;
	FileTransferWindowActionDescription = 0;
}

void FileTransferModule::sendFileActionActivated(QAction *sender, bool toggled)
{
	kdebugf();

	KaduMainWindow *kaduMainWindow = dynamic_cast<KaduMainWindow *>(sender->parent());
	if (!kaduMainWindow)
		return;

	ContactList contacts = kaduMainWindow->contacts();
	if (contacts.count())
		selectFilesAndSend(contacts);

	kdebugf2();
}

void FileTransferModule::toggleFileTransferWindow(QAction *sender, bool toggled)
{
	if (Window)
	{
		disconnect(Window, SIGNAL(destroyed()), this, SLOT(fileTransferWindowDestroyed()));
		delete Window;
		Window = 0;
	}
	else
	{
		Window = new FileTransferWindow();
		connect(Window, SIGNAL(destroyed()), this, SLOT(fileTransferWindowDestroyed()));
		Window->show();
	}
}

void FileTransferModule::showFileTransferWindow()
{
	if (!Window)
		toggleFileTransferWindow(0, true);
}

void FileTransferModule::fileTransferWindowDestroyed()
{
	Window = 0;
}

QStringList FileTransferModule::selectFilesToSend()
{
	return QFileDialog::getOpenFileNames(
		QString::null,
		config_file.readEntry("Network", "LastUploadDirectory"),
		0, "open file", tr("Select file location"));
}

void FileTransferModule::selectFilesAndSend(ContactList contacts)
{
	QStringList files = selectFilesToSend();
	if (!files.count())
	{
		kdebugf2();
		return;
	}

	foreach (const Contact &contact, contacts)
	{
		Account *account = contact.prefferedAccount();
		FileTransferService *service = account->protocol()->fileTransferService();
		if (!service)
			continue;

		foreach (const QString &file, files)
		{
			FileTransfer *fileTransfer = service->createOutgoingFileTransfer(contact);
			fileTransfer->setLocalFileName(file);
			fileTransfer->send();

			FileTransferManager::instance()->addFileTransfer(fileTransfer);
			showFileTransferWindow();
		}
	}
}
