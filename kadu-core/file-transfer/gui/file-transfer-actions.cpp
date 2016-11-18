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

#include "file-transfer-actions.h"

#include "core/injected-factory.h"
#include "file-transfer/gui/send-file-action.h"
#include "file-transfer/gui/show-file-transfer-window-action.h"
#include "gui/actions/actions.h"
#include "gui/menu/menu-inventory.h"

FileTransferActions::FileTransferActions(QObject *parent) :
		QObject{parent}
{
}

FileTransferActions::~FileTransferActions()
{
}

void FileTransferActions::setActions(Actions *actions)
{
	m_actions = actions;
}

void FileTransferActions::setMenuInventory(MenuInventory *menuInventory)
{
	m_menuInventory = menuInventory;
}

void FileTransferActions::setSendFileAction(SendFileAction *sendFileAction)
{
	m_sendFileAction = sendFileAction;
}

void FileTransferActions::setShowFileTransferWindowAction(ShowFileTransferWindowAction *showFileTransferWindowAction)
{
	m_showFileTransferWindowAction = showFileTransferWindowAction;
}

void FileTransferActions::init()
{
	m_actions->insert(m_sendFileAction);
	m_actions->insert(m_showFileTransferWindowAction);

	m_menuInventory
		->menu("buddy-list")
		->addAction(m_sendFileAction, KaduMenu::SectionSend, 100);

	m_menuInventory
		->menu("tools")
		->addAction(m_showFileTransferWindowAction, KaduMenu::SectionTools, 5);
}

void FileTransferActions::done()
{
	m_menuInventory
		->menu("buddy-list")
		->removeAction(m_sendFileAction);

	m_menuInventory
		->menu("tools")
		->removeAction(m_showFileTransferWindowAction);

	m_actions->remove(m_sendFileAction);
	m_actions->remove(m_showFileTransferWindowAction);
}

#include "moc_file-transfer-actions.cpp"
