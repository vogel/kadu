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

#include "gui-module.h"

#include "gui/actions/actions.h"
#include "gui/configuration/chat-configuration-holder.h"
#include "gui/menu/menu-inventory.h"
#include "gui/widgets/chat-widget/chat-widget-actions.h"
#include "gui/widgets/webkit-messages-view/webkit-messages-view-display-factory.h"
#include "gui/widgets/webkit-messages-view/webkit-messages-view-factory.h"
#include "gui/widgets/webkit-messages-view/webkit-messages-view-handler-factory.h"
#include "gui/windows/buddy-data-window-repository.h"
#include "gui/widgets/chat-edit-box-size-manager.h"
#include "gui/windows/chat-data-window-repository.h"
#include "gui/widgets/custom-input-menu-manager.h"

GuiModule::GuiModule()
{
	add_type<Actions>();
	add_type<BuddyDataWindowRepository>();
	add_type<ChatConfigurationHolder>();
	add_type<ChatDataWindowRepository>();
	add_type<ChatEditBoxSizeManager>();
	add_type<ChatWidgetActions>();
	add_type<CustomInputMenuManager>();
	add_type<MenuInventory>();
	add_type<WebkitMessagesViewDisplayFactory>();
	add_type<WebkitMessagesViewFactory>();
	add_type<WebkitMessagesViewHandlerFactory>();
}

GuiModule::~GuiModule()
{
}
