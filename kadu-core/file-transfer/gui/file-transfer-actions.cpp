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

#include "file-transfer-actions.h"

#include "file-transfer/gui/send-file-action.h"
#include "file-transfer/gui/show-file-transfer-window-action.h"
#include "gui/menu/menu-inventory.h"

FileTransferActions::FileTransferActions(QObject *parent) :
		QObject{parent}
{
	m_sendFileAction = new SendFileAction{this};
	m_showFileTransferWindow = new ShowFileTransferWindowAction{this};

	MenuInventory::instance()
		->menu("buddy-list")
		->addAction(m_sendFileAction, KaduMenu::SectionSend, 100);

	MenuInventory::instance()
		->menu("tools")
		->addAction(m_showFileTransferWindow, KaduMenu::SectionTools, 5);
}

FileTransferActions::~FileTransferActions()
{
	MenuInventory::instance()
		->menu("buddy-list")
		->removeAction(m_sendFileAction);

	MenuInventory::instance()
		->menu("tools")
		->removeAction(m_showFileTransferWindow);
}

#include "moc_file-transfer-actions.cpp"
