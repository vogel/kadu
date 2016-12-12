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

#include "actions/actions.h"
#include "gui/configuration/chat-configuration-holder.h"
#include "gui/menu/menu-inventory.h"
#include "gui/widgets/chat-edit-box-size-manager.h"
#include "gui/widgets/chat-widget/actions/chat-widget-actions.h"
#include "gui/widgets/webkit-messages-view/webkit-messages-view-display-factory.h"
#include "gui/widgets/webkit-messages-view/webkit-messages-view-factory.h"
#include "gui/widgets/webkit-messages-view/webkit-messages-view-handler-factory.h"
#include "gui/windows/buddy-data-window-repository.h"
#include "gui/windows/chat-data-window-repository.h"
#include "gui/windows/kadu-window-actions.h"
#include "gui/windows/kadu-window-service.h"
#include "gui/windows/main-configuration-window-service.h"
#include "gui/windows/main-window-repository.h"
#include "gui/windows/multilogon-window-service.h"
#include "gui/windows/open-chat-with/open-chat-with-service.h"
#include "gui/windows/proxy-edit-window-service.h"
#include "gui/windows/your-accounts-window-service.h"
#include "gui/widgets/custom-input-menu-manager.h"
#include "gui/widgets/tool-tip-manager.h"
#include "gui/window-manager.h"

GuiModule::GuiModule()
{
	add_type<Actions>();
	add_type<BuddyDataWindowRepository>();
	add_type<ChatConfigurationHolder>();
	add_type<ChatDataWindowRepository>();
	add_type<ChatEditBoxSizeManager>();
	add_type<ChatWidgetActions>();
	add_type<CustomInputMenuManager>();
	add_type<KaduWindowActions>();
	add_type<KaduWindowService>();
	add_type<MainConfigurationWindowService>();
	add_type<MainWindowRepository>();
	add_type<MenuInventory>();
	add_type<MultilogonWindowService>();
	add_type<OpenChatWithService>();
	add_type<ProxyEditWindowService>();
	add_type<ToolTipManager>();
	add_type<WebkitMessagesViewDisplayFactory>();
	add_type<WebkitMessagesViewFactory>();
	add_type<WebkitMessagesViewHandlerFactory>();
	add_type<WindowManager>();
	add_type<YourAccountsWindowService>();
}

GuiModule::~GuiModule()
{
}
