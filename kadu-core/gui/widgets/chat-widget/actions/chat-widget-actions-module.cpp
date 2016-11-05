/*
 * %kadu copyright begin%
 * Copyright 2014, 2015 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "chat-widget-actions-module.h"

#include "gui/widgets/chat-widget/actions/auto-send-action.h"
#include "gui/widgets/chat-widget/actions/block-user-action.h"
#include "gui/widgets/chat-widget/actions/bold-action.h"
#include "gui/widgets/chat-widget/actions/clear-chat-action.h"
#include "gui/widgets/chat-widget/actions/insert-image-action.h"
#include "gui/widgets/chat-widget/actions/italic-action.h"
#include "gui/widgets/chat-widget/actions/more-actions-action.h"
#include "gui/widgets/chat-widget/actions/open-chat-action.h"
#include "gui/widgets/chat-widget/actions/open-chat-with-action.h"
#include "gui/widgets/chat-widget/actions/send-action.h"
#include "gui/widgets/chat-widget/actions/underline-action.h"

ChatWidgetActionsModule::ChatWidgetActionsModule()
{
	add_type<AutoSendAction>();
	add_type<BlockUserAction>();
	add_type<BoldAction>();
	add_type<ClearChatAction>();
	add_type<InsertImageAction>();
	add_type<ItalicAction>();
	add_type<MoreActionsAction>();
	add_type<OpenChatAction>();
	add_type<OpenChatWithAction>();
	add_type<SendAction>();
	add_type<UnderlineAction>();
}

ChatWidgetActionsModule::~ChatWidgetActionsModule()
{
}
