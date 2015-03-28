/*
 * %kadu copyright begin%
 * Copyright 2015 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "send-file-action.h"

#include "accounts/account.h"
#include "chat/chat.h"
#include "configuration/deprecated-configuration-api.h"
#include "contacts/contact-set.h"
#include "core/application.h"
#include "core/core.h"
#include "file-transfer/file-transfer-direction.h"
#include "file-transfer/file-transfer-handler.h"
#include "file-transfer/file-transfer-manager.h"
#include "file-transfer/file-transfer.h"
#include "file-transfer/gui/file-transfer-can-send-result.h"
#include "gui/actions/action-context.h"
#include "gui/actions/action.h"
#include "protocols/protocol.h"
#include "protocols/services/file-transfer-service.h"

#include <QtWidgets/QFileDialog>

SendFileAction::SendFileAction(QObject *parent) :
		// using C++ initializers breaks Qt's lupdate
		ActionDescription(parent)
{
	setIcon(KaduIcon("document-send"));
	setName("sendFileAction");
	setText(tr("Send File..."));
	setType(ActionDescription::TypeUser);

	registerAction();
}

SendFileAction::~SendFileAction()
{
}

void SendFileAction::actionInstanceCreated(Action *action)
{
	auto account = action->context()->chat().chatAccount();
	if (!account || !account.protocolHandler() || !account.protocolHandler()->fileTransferService())
		return;

	connect(account.protocolHandler()->fileTransferService(), SIGNAL(canSendChanged()), action, SLOT(checkState()));
}

void SendFileAction::triggered(QWidget* widget, ActionContext* context, bool toggled)
{
	Q_UNUSED(widget)
	Q_UNUSED(toggled)

	if (!context)
		return;

	auto contacts = context->contacts();
	if (!contacts.isEmpty())
		selectFilesAndSend(contacts);
}

void SendFileAction::updateActionState(Action *action)
{
	action->setEnabled(false);
	action->setToolTip(text());

	if (action->context()->buddies().isAnyTemporary())
		return;

	auto contacts = action->context()->contacts();

	if (contacts.isEmpty())
		return;

	for (auto &contact : contacts)
	{
		if (Core::instance()->myself() == contact.ownerBuddy())
			return;

		auto account = contact.contactAccount();
		if (account.isNull() || !account.protocolHandler() || !account.protocolHandler()->fileTransferService())
			return;

		auto canSend = account.protocolHandler()->fileTransferService()->canSend(contact);
		if (!canSend.canSend())
		{
			if (!canSend.reason().isEmpty())
				action->setToolTip(canSend.reason());
			return;
		}
	}

	action->setEnabled(true);
}

void SendFileAction::selectFilesAndSend(const ContactSet &contacts)
{
	auto filesNames = selectFilesToSend();
	if (filesNames.isEmpty())
		return;

	for (auto &&contact : contacts)
	{
		auto account = contact.contactAccount();
		if (!account.protocolHandler())
			continue;

		auto service = account.protocolHandler()->fileTransferService();
		if (!service)
			continue;

		for (auto &&fileName : filesNames)
		{
			auto fileTransfer = FileTransfer::create();
			fileTransfer.setPeer(contact);
			fileTransfer.setTransferDirection(FileTransferDirection::Outgoing);

			Core::instance()->fileTransferManager()->addItem(fileTransfer);
			Core::instance()->fileTransferManager()->sendFile(fileTransfer, fileName);
			Core::instance()->fileTransferManager()->showFileTransferWindow();
		}
	}
}

QStringList SendFileAction::selectFilesToSend() const
{
	return QFileDialog::getOpenFileNames(
			nullptr, tr("Select file location"),
			Application::instance()->configuration()->deprecatedApi()->readEntry("Network", "LastUploadDirectory"));
}

#include "moc_send-file-action.cpp"
