/*
 * %kadu copyright begin%
 * Copyright 2009, 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009, 2010, 2012 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2010, 2011 Piotr Dąbrowski (ultr@ultr.pl)
 * Copyright 2009 Michał Podsiadlik (michal@kadu.net)
 * Copyright 2009 Bartłomiej Zimoń (uzi18@o2.pl)
 * Copyright 2010 Radosław Szymczyszyn (lavrin@gmail.com)
 * Copyright 2010, 2011, 2012, 2013, 2014 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2009, 2010, 2011, 2012, 2013, 2014, 2015 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2009 Longer (longer89@gmail.com)
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

#include "kadu-window-actions.h"

#include "actions/chat-widget/block-user-action.h"
#include "actions/talkable/copy-description-action.h"
#include "actions/talkable/copy-personal-info-action.h"
#include "actions/talkable/merge-buddies-action.h"
#include "actions/talkable/open-buddy-email-action.h"
#include "actions/talkable/open-description-link-action.h"
#include "actions/talkable/delete-talkable-action.h"
#include "actions/talkable/edit-talkable-action.h"
#include "actions/tree-view/collapse-action.h"
#include "actions/tree-view/expand-action.h"
#include "gui/menu/menu-inventory.h"

KaduWindowActions::KaduWindowActions(QObject *parent) : QObject(parent)
{
}

KaduWindowActions::~KaduWindowActions()
{
}

void KaduWindowActions::setBlockUserAction(BlockUserAction *blockUserAction)
{
	m_blockUserAction = blockUserAction;
}

void KaduWindowActions::setCollapseAction(CollapseAction *collapseAction)
{
	m_collapseAction = collapseAction;
}

void KaduWindowActions::setCopyDescriptionAction(CopyDescriptionAction *copyDescriptionAction)
{
	m_copyDescriptionAction = copyDescriptionAction;
}

void KaduWindowActions::setCopyPersonalInfoAction(CopyPersonalInfoAction *copyPersonalInfoAction)
{
	m_copyPersonalInfoAction = copyPersonalInfoAction;
}

void KaduWindowActions::setDeleteTalkableAction(DeleteTalkableAction *deleteTalkableAction)
{
	m_deleteTalkableAction = deleteTalkableAction;
}

void KaduWindowActions::setEditTalkableAction(EditTalkableAction *editTalkableAction)
{
	m_editTalkableAction = editTalkableAction;
}

void KaduWindowActions::setExpandAction(ExpandAction *expandAction)
{
	m_expandAction = expandAction;
}

void KaduWindowActions::setMenuInventory(MenuInventory *menuInventory)
{
	m_menuInventory = menuInventory;
}

void KaduWindowActions::setMergeBuddiesAction(MergeBuddiesAction *mergeBuddiesAction)
{
	m_mergeBuddiesAction = mergeBuddiesAction;
}

void KaduWindowActions::setOpenBuddyEmailAction(OpenBuddyEmailAction *openBuddyEmailAction)
{
	m_openBuddyEmailAction = openBuddyEmailAction;
}

void KaduWindowActions::setOpenDescriptionLinkAction(OpenDescriptionLinkAction *openDescriptionLinkAction)
{
	m_openDescriptionLinkAction = openDescriptionLinkAction;
}

void KaduWindowActions::init()
{
	m_menuInventory
		->menu("buddy-list")
		->addAction(m_expandAction, KaduMenu::SectionActionsGui, 2);
	m_menuInventory
		->menu("buddy-list")
		->addAction(m_collapseAction, KaduMenu::SectionActionsGui, 1);

	m_menuInventory
		->menu("buddy-list")
		->addAction(m_copyDescriptionAction, KaduMenu::SectionActions, 10);

	m_menuInventory
		->menu("buddy-list")
		->addAction(m_copyPersonalInfoAction, KaduMenu::SectionActions, 20);

	m_menuInventory
		->menu("buddy-list")
		->addAction(m_openDescriptionLinkAction, KaduMenu::SectionActions, 30);

	m_menuInventory
		->menu("buddy-list")
		->addAction(m_openBuddyEmailAction, KaduMenu::SectionSend, 200);

	m_menuInventory
		->menu("buddy-list")
		->addAction(m_editTalkableAction, KaduMenu::SectionView);

	m_menuInventory
		->menu("buddy-list")
		->addAction(m_mergeBuddiesAction, KaduMenu::SectionManagement, 100);

	m_menuInventory
		->menu("buddy-list")
		->addAction(m_blockUserAction, KaduMenu::SectionManagement, 500);

	m_menuInventory
		->menu("buddy-list")
		->addAction(m_deleteTalkableAction, KaduMenu::SectionManagement, 1000);
}

#include "moc_kadu-window-actions.cpp"
