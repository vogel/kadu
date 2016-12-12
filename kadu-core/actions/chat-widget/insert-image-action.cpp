/*
 * %kadu copyright begin%
 * Copyright 2016 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "insert-image-action.h"

#include "actions/action.h"
#include "protocols/protocol.h"
#include "widgets/chat-edit-box.h"

#include <QtWidgets/QMenu>

InsertImageAction::InsertImageAction(QObject *parent) :
		// using C++ initializers breaks Qt's lupdate
		ActionDescription(parent)
{
	setIcon(KaduIcon{"insert-image"});
	setName(QStringLiteral("insertImageAction"));
	setText(tr("Insert Image"));
	setType(ActionDescription::TypeChat);
}

InsertImageAction::~InsertImageAction()
{
}

void InsertImageAction::actionTriggered(QAction *sender, bool)
{
	auto chatEditBox = qobject_cast<ChatEditBox *>(sender->parent());
	if (!chatEditBox)
		return;

	chatEditBox->openInsertImageDialog();
}

void InsertImageAction::updateActionState(Action* action)
{
	action->setEnabled(false);

	auto chatEditBox = qobject_cast<ChatEditBox *>(action->parent());
	if (!chatEditBox)
		return;

	auto account = action->context()->chat().chatAccount();
	if (!account)
		return;

	auto protocol = account.protocolHandler();
	if (!protocol)
		return;

	action->setEnabled(protocol->chatImageService());
}

#include "moc_insert-image-action.cpp"
