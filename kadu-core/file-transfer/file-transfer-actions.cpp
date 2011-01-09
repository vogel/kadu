/*
 * %kadu copyright begin%
 * Copyright 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2010 Piotr Galiszewski (piotrgaliszewski@gmail.com)
 * Copyright 2010 Bartłomiej Zimoń (uzi18@o2.pl)
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

#include <QtGui/QFileDialog>

#include "configuration/configuration-file.h"
#include "contacts/contact-set.h"
#include "core/core.h"
#include "file-transfer/file-transfer-handler.h"
#include "file-transfer/file-transfer-manager.h"
#include "gui/actions/action.h"
#include "gui/actions/action-description.h"
#include "gui/widgets/buddies-list-view-menu-manager.h"
#include "gui/windows/kadu-window.h"
#include "protocols/services/file-transfer-service.h"
#include "protocols/protocol.h"
#include "debug.h"

#include "file-transfer-actions.h"

void disableNonFileTransferContacts(Action *action)
{
	kdebugf();

	action->setEnabled(false);

	const ContactSet &contacts = action->contacts();

	if (!contacts.count())
		return;

	foreach (const Contact &contact, contacts)
	{
		if (Core::instance()->myself() == contact.ownerBuddy())
			return;

		Account account = contact.contactAccount();
		if (account.isNull() || !account.protocolHandler() || !account.protocolHandler()->fileTransferService())
			return;
	}

	action->setEnabled(true);
}

FileTransferActions::FileTransferActions(QObject *parent)
		: QObject(parent)
{
	SendFileActionDescription = new ActionDescription(this,
		ActionDescription::TypeUser, "sendFileAction",
		this, SLOT(sendFileActionActivated(QAction *, bool)),
		"document-send", tr("Send File..."), false,
		disableNonFileTransferContacts
	);
	SendFileActionDescription->setShortcut("kadu_sendfile");
	BuddiesListViewMenuManager::instance()->addActionDescription(SendFileActionDescription, BuddiesListViewMenuItem::MenuCategoryActions, 100);

	FileTransferWindowActionDescription = new ActionDescription(this,
		ActionDescription::TypeMainMenu, "sendFileWindowAction",
		this, SLOT(toggleFileTransferWindow(QAction *, bool)),
		"document-send", tr("View File Transfers")
	);

	Core::instance()->kaduWindow()->insertMenuActionDescription(FileTransferWindowActionDescription, KaduWindow::MenuTools, 5);
}

FileTransferActions::~FileTransferActions()
{
	BuddiesListViewMenuManager::instance()->removeActionDescription(SendFileActionDescription);
	Core::instance()->kaduWindow()->removeMenuActionDescription(FileTransferWindowActionDescription);
}

void FileTransferActions::sendFileActionActivated(QAction *sender, bool toggled)
{
	Q_UNUSED(toggled)

	kdebugf();

	Action *action = dynamic_cast<Action *>(sender);
	if (!action)
		return;

	ContactSet contacts = action->contacts();
	if (contacts.count())
		selectFilesAndSend(contacts);

	kdebugf2();
}

void FileTransferActions::toggleFileTransferWindow(QAction *sender, bool toggled)
{
	Q_UNUSED(sender)
	Q_UNUSED(toggled)

	if (FileTransferManager::instance()->isFileTransferWindowVisible())
		FileTransferManager::instance()->hideFileTransferWindow();
	else
		FileTransferManager::instance()->showFileTransferWindow();
}

QStringList FileTransferActions::selectFilesToSend()
{
	return QFileDialog::getOpenFileNames(
			0, tr("Select file location"),
			config_file.readEntry("Network", "LastUploadDirectory"));
}

void FileTransferActions::selectFilesAndSend(const ContactSet &contacts)
{
	QStringList files = selectFilesToSend();
	if (!files.count())
	{
		kdebugf2();
		return;
	}

	foreach (const Contact &contact, contacts)
	{
		Account account = contact.contactAccount();
		if (!account.protocolHandler())
			continue;

		FileTransferService *service = account.protocolHandler()->fileTransferService();
		if (!service)
			continue;

		foreach (const QString &file, files)
		{
			FileTransfer fileTransfer = FileTransfer::create();
			fileTransfer.setPeer(contact);
			fileTransfer.setTransferType(TypeSend);
			fileTransfer.setLocalFileName(file);

			FileTransferManager::instance()->addItem(fileTransfer);

			fileTransfer.createHandler();
			if (fileTransfer.handler())
				fileTransfer.handler()->send();

			FileTransferManager::instance()->showFileTransferWindow();
		}
	}
}
