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
#include <QtGui/QMessageBox>

#include "accounts/account.h"
#include "configuration/configuration-file.h"
#include "configuration/xml-configuration-file.h"
#include "contacts/contact.h"
#include "core/core.h"
#include "file-transfer/file-transfer.h"
#include "file-transfer/file-transfer-manager.h"
#include "protocols/protocol.h"
#include "protocols/services/file-transfer-service.h"
#include "gui/actions/action.h"
#include "gui/widgets/contacts-list-view-menu-manager.h"
#include "gui/windows/kadu-window.h"
#include "gui/windows/message-box.h"

#include "debug.h"
#include "exports.h"

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

void disableNonFileTransferContacts(Action *action)
{
	kdebugf();

	action->setEnabled(false);

	const ContactSet &contacts = action->contacts();

	if (!contacts.count())
		return;

	foreach (const Contact &contact, contacts)
	{
		if (Core::instance()->myself() == contact)
			return;

		Account *account = contact.prefferedAccount();
		if (!account || !account->protocol()->fileTransferService())
			return;
	}

	action->setEnabled(true);
}

FileTransferModule::FileTransferModule() :
		Window(0)
{
	createActionDecriptions();

	connect(FileTransferManager::instance(), SIGNAL(incomingFileTransferNeedAccept(FileTransfer *)),
		this, SLOT(incomingFileTransferNeedAccept(FileTransfer *)));
}

FileTransferModule::~FileTransferModule()
{
	disconnect(FileTransferManager::instance(), SIGNAL(incomingFileTransferNeedAccept(FileTransfer *)),
		this, SLOT(incomingFileTransferNeedAccept(FileTransfer *)));

	deleteActionDecriptions();

	if (Window)
	{
		delete Window;
		Window = 0;
	}
}

void FileTransferModule::createActionDecriptions()
{
	SendFileActionDescription = new ActionDescription(0,
		ActionDescription::TypeUser, "sendFileAction",
		this, SLOT(sendFileActionActivated(QAction *, bool)),
		"SendFile", tr("Send file"), false, QString::null,
		disableNonFileTransferContacts
	);
	SendFileActionDescription->setShortcut("kadu_sendfile");
	ContactsListViewMenuManager::instance()->insertActionDescription(1, SendFileActionDescription);

	FileTransferWindowActionDescription = new ActionDescription(0,
		ActionDescription::TypeMainMenu, "sendFileWindowAction",
		this, SLOT(toggleFileTransferWindow(QAction *, bool)),
		"SendFileWindow", tr("File transfers...")
	);

	Core::instance()->kaduWindow()->insertMenuActionDescription(FileTransferWindowActionDescription, KaduWindow::MenuKadu, 5);
}

void FileTransferModule::deleteActionDecriptions()
{
	ContactsListViewMenuManager::instance()->removeActionDescription(SendFileActionDescription);
	// TODO: 0.6.6
// 	Core::instance()->kaduWindow()->removeMenuActionDescription(FileTransferWindowActionDescription);

	delete SendFileActionDescription;
	SendFileActionDescription = 0;
	delete FileTransferWindowActionDescription;
	FileTransferWindowActionDescription = 0;
}

void FileTransferModule::sendFileActionActivated(QAction *sender, bool toggled)
{
	kdebugf();

	MainWindow *kaduMainWindow = dynamic_cast<MainWindow *>(sender->parent());
	if (!kaduMainWindow)
		return;

	ContactSet contacts = kaduMainWindow->contacts();
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
			0, tr("Select file location"),
			config_file.readEntry("Network", "LastUploadDirectory"));
}

void FileTransferModule::selectFilesAndSend(ContactSet contacts)
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

void FileTransferModule::incomingFileTransferNeedAccept(FileTransfer *fileTransfer)
{
	QString fileName;

	bool resume = false;
	bool haveFileName = false;

	QFileInfo fi;

	while (fileName.isEmpty())
	{
		if (!haveFileName || fileName.isEmpty())
			fileName = QFileDialog::getSaveFileName(Core::instance()->kaduWindow(), tr("Select file location"),
					config_file.readEntry("Network", "LastDownloadDirectory") + fileTransfer->remoteFileName(),
					QString::null, 0, QFileDialog::DontConfirmOverwrite);

		if (fileName.isEmpty())
		{
			kdebugmf(KDEBUG_INFO, "rejected\n");
			fileTransfer->reject();
			return;
		}

		config_file.writeEntry("Network", "LastDownloadDirectory", QFileInfo(fileName).absolutePath() + '/');
		fi.setFile(fileName);

		if (!haveFileName && fi.exists())
		{
			QString question;
			question = tr("File %1 already exists.").arg(fileName);

			switch (QMessageBox::question(0, tr("Save file"), question, tr("Overwrite"), tr("Resume"),
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

		QFile file(fileName);
		QIODevice::OpenMode flags = QIODevice::WriteOnly;
		if (resume)
			flags |= QIODevice::Append;
		else
			flags |= QIODevice::Truncate;

		if (!file.open(flags))
			MessageBox::msg(tr("Could not open file. Select another one."), true, "Warning");
		else
		{
			fileTransfer->accept(file);
			showFileTransferWindow();
		}
	}

	FileTransferManager::instance()->addFileTransfer(fileTransfer);
}
