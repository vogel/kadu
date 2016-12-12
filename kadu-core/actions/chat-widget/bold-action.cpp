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

#include "bold-action.h"

#include "actions/action.h"
#include "protocols/protocol-factory.h"
#include "protocols/protocol.h"
#include "widgets/chat-edit-box.h"
#include "widgets/chat-widget/chat-widget.h"
#include "widgets/custom-input.h"

BoldAction::BoldAction(QObject *parent) :
		// using C++ initializers breaks Qt's lupdate
		ActionDescription(parent)
{
    setCheckable(true);
	setIcon(KaduIcon{"format-text-bold"});
	setName(QStringLiteral("boldAction"));
	setText(tr("Bold"));
	setType(ActionDescription::TypeChat);
}

BoldAction::~BoldAction()
{
}

void BoldAction::actionTriggered(QAction *sender, bool toggled)
{
	auto chatEditBox = qobject_cast<ChatEditBox *>(sender->parent());
	if (!chatEditBox)
		return;

	chatEditBox->inputBox()->setFontWeight(toggled ? QFont::Bold : QFont::Normal);
}

void BoldAction::updateActionState(Action* action)
{
	action->setEnabled(false);

	auto chat = action->context()->chat();
	if (!chat)
		return;

	auto protocol = chat.chatAccount().protocolHandler();
	if (!protocol)
		return;

	if (!protocol->protocolFactory())
		return;

	action->setEnabled(protocol->protocolFactory()->name() == "gadu");
}

#include "moc_bold-action.cpp"
