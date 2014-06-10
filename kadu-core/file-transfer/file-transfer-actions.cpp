/*
 * %kadu copyright begin%
 * Copyright 2010, 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2010, 2012 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2010 Bartłomiej Zimoń (uzi18@o2.pl)
 * Copyright 2009, 2010, 2011, 2012, 2013 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2010, 2011, 2013 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#include <QtWidgets/QFileDialog>

#include "configuration/configuration.h"
#include "configuration/deprecated-configuration-api.h"
#include "contacts/contact-set.h"
#include "core/application.h"
#include "core/core.h"
#include "file-transfer/file-transfer-handler.h"
#include "file-transfer/file-transfer-manager.h"
#include "gui/actions/action-context.h"
#include "gui/actions/action-description.h"
#include "gui/actions/action.h"
#include "gui/menu/menu-inventory.h"
#include "protocols/protocol.h"
#include "protocols/services/file-transfer-service.h"
#include "debug.h"

#include "file-transfer-actions.h"

void disableNonFileTransferContacts(Action *action)
{
	kdebugf();

	action->setEnabled(false);

	const ContactSet &contacts = action->context()->contacts();

	if (contacts.isEmpty())
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
		KaduIcon("document-send"), tr("Send File..."), false,
		disableNonFileTransferContacts
	);
	SendFileActionDescription->setShortcut("kadu_sendfile");

	connect(SendFileActionDescription, SIGNAL(actionCreated(Action*)),
	        this, SLOT(sendFileActionCreated(Action*)));

	FileTransferWindowActionDescription = new ActionDescription(this,
		ActionDescription::TypeMainMenu, "sendFileWindowAction",
		this, SLOT(toggleFileTransferWindow(QAction *, bool)),
		KaduIcon("document-send"), tr("View File Transfers")
	);

	MenuInventory::instance()
		->menu("buddy-list")
		->addAction(SendFileActionDescription, KaduMenu::SectionSend, 100);

	MenuInventory::instance()
		->menu("tools")
		->addAction(FileTransferWindowActionDescription, KaduMenu::SectionTools, 5);
}

FileTransferActions::~FileTransferActions()
{
	MenuInventory::instance()
		->menu("buddy-list")
		->removeAction(SendFileActionDescription);

	MenuInventory::instance()
		->menu("tools")
		->removeAction(FileTransferWindowActionDescription);
}

void FileTransferActions::sendFileActionCreated(Action *action)
{
	const Account &account = action->context()->chat().chatAccount();
	if (!account)
		return;

	connect(account, SIGNAL(fileTransferServiceRegistered()), action, SLOT(checkState()));
	connect(account, SIGNAL(fileTransferServiceUnregistered()), action, SLOT(checkState()));
}

void FileTransferActions::sendFileActionActivated(QAction *sender, bool toggled)
{
	Q_UNUSED(toggled)

	kdebugf();

	Action *action = qobject_cast<Action *>(sender);
	if (!action)
		return;

	ContactSet contacts = action->context()->contacts();
	if (!contacts.isEmpty())
		selectFilesAndSend(contacts);

	kdebugf2();
}

void FileTransferActions::toggleFileTransferWindow(QAction *sender, bool toggled)
{
	Q_UNUSED(sender)
	Q_UNUSED(toggled)

	FileTransferManager::instance()->showFileTransferWindow();
}

QStringList FileTransferActions::selectFilesToSend()
{
	return QFileDialog::getOpenFileNames(
			0, tr("Select file location"),
			Application::instance()->configuration()->deprecatedApi()->readEntry("Network", "LastUploadDirectory"));
}

void FileTransferActions::selectFilesAndSend(const ContactSet &contacts)
{
	QStringList files = selectFilesToSend();
	if (files.isEmpty())
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

#include "moc_file-transfer-actions.cpp"
