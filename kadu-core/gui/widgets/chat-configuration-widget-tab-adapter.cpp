/*
 * %kadu copyright begin%
 * Copyright 2013 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include <QtGui/QTabWidget>

#include "gui/widgets/chat-configuration-widget.h"
#include "gui/windows/chat-data-window.h"

#include "chat-configuration-widget-tab-adapter.h"

ChatConfigurationWidgetTabAdapter::ChatConfigurationWidgetTabAdapter(ChatDataWindow *chatDataWindow, QTabWidget *tabWidget, QObject *parent) :
		QObject(parent), MyChatDataWindow(chatDataWindow), MyTabWidget(tabWidget)
{
	if (!MyChatDataWindow || !MyTabWidget)
		return;

	connect(MyChatDataWindow, SIGNAL(widgetAdded(ChatConfigurationWidget*)), this, SLOT(widgetAdded(ChatConfigurationWidget*)));

	foreach (ChatConfigurationWidget *widget, MyChatDataWindow->chatConfigurationWidgets())
		widgetAdded(widget);
}

ChatConfigurationWidgetTabAdapter::~ChatConfigurationWidgetTabAdapter()
{
}

void ChatConfigurationWidgetTabAdapter::widgetAdded(ChatConfigurationWidget *widget)
{
	MyTabWidget->addTab(widget, widget->windowTitle());
}

#include "moc_chat-configuration-widget-tab-adapter.cpp"
