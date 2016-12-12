/*
 * %kadu copyright begin%
 * Copyright 2009, 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2012 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2011 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2009, 2010, 2011, 2012, 2013, 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#pragma once

#include "injeqt-type-roles.h"

#include <QtCore/QObject>
#include <QtCore/QPointer>
#include <injeqt/injeqt.h>

class BlockUserAction;
class CollapseAction;
class CopyDescriptionAction;
class CopyPersonalInfoAction;
class DeleteTalkableAction;
class EditTalkableAction;
class ExpandAction;
class MenuInventory;
class MergeBuddiesAction;
class OpenBuddyEmailAction;
class OpenDescriptionLinkAction;

class KaduWindowActions : public QObject
{
	Q_OBJECT
	INJEQT_TYPE_ROLE(SERVICE)

	QPointer<BlockUserAction> m_blockUserAction;
	QPointer<CollapseAction> m_collapseAction;
	QPointer<CopyDescriptionAction> m_copyDescriptionAction;
	QPointer<CopyPersonalInfoAction> m_copyPersonalInfoAction;
	QPointer<DeleteTalkableAction> m_deleteTalkableAction;
	QPointer<EditTalkableAction> m_editTalkableAction;
	QPointer<ExpandAction> m_expandAction;
	QPointer<MenuInventory> m_menuInventory;
	QPointer<MergeBuddiesAction> m_mergeBuddiesAction;
	QPointer<OpenBuddyEmailAction> m_openBuddyEmailAction;
	QPointer<OpenDescriptionLinkAction> m_openDescriptionLinkAction;

private slots:
	INJEQT_SET void setBlockUserAction(BlockUserAction *blockUserAction);
	INJEQT_SET void setCollapseAction(CollapseAction *collapseAction);
	INJEQT_SET void setCopyDescriptionAction(CopyDescriptionAction *copyDescriptionAction);
	INJEQT_SET void setCopyPersonalInfoAction(CopyPersonalInfoAction *copyPersonalInfoAction);
	INJEQT_SET void setDeleteTalkableAction(DeleteTalkableAction *deleteTalkableAction);
	INJEQT_SET void setEditTalkableAction(EditTalkableAction *editTalkableAction);
	INJEQT_SET void setExpandAction(ExpandAction *expandAction);
	INJEQT_SET void setMenuInventory(MenuInventory *menuInventory);
	INJEQT_SET void setMergeBuddiesAction(MergeBuddiesAction *mergeBuddiesAction);
	INJEQT_SET void setOpenBuddyEmailAction(OpenBuddyEmailAction *openBuddyEmailAction);
	INJEQT_SET void setOpenDescriptionLinkAction(OpenDescriptionLinkAction *openDescriptionLinkAction);
	INJEQT_INIT void init();

public:
	Q_INVOKABLE KaduWindowActions(QObject *parent = nullptr);
	virtual ~KaduWindowActions();

};

