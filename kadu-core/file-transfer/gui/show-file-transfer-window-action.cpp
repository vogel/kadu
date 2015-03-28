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

#include "show-file-transfer-window-action.h"

#include "core/core.h"
#include "file-transfer/file-transfer-manager.h"

ShowFileTransferWindowAction::ShowFileTransferWindowAction(QObject *parent) :
		// using C++ initializers breaks Qt's lupdate
		ActionDescription(parent)
{
	setIcon(KaduIcon("document-send"));
	setName("sendFileWindowAction");
	setText(tr("View File Transfers"));
	setType(ActionDescription::TypeMainMenu);

	registerAction();
}

ShowFileTransferWindowAction::~ShowFileTransferWindowAction()
{
}

void ShowFileTransferWindowAction::triggered(QWidget *widget, ActionContext *context, bool toggled)
{
	Q_UNUSED(widget)
	Q_UNUSED(context)
	Q_UNUSED(toggled)

	Core::instance()->fileTransferManager()->showFileTransferWindow();
}

#include "moc_show-file-transfer-window-action.cpp"
