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

#include "single-window-plugin-object.h"

#include "single-window-chat-widget-container-handler.h"
#include "single-window.h"

#include "gui/widgets/chat-widget/chat-widget-container-handler-repository.h"
#include "gui/windows/main-configuration-window.h"
#include "misc/paths-provider.h"

SingleWindowPluginObject::SingleWindowPluginObject(QObject *parent) :
		QObject{parent}
{
}

SingleWindowPluginObject::~SingleWindowPluginObject()
{
}

void SingleWindowPluginObject::setChatWidgetContainerHandlerRepository(ChatWidgetContainerHandlerRepository *chatWidgetContainerHandlerRepository)
{
	m_chatWidgetContainerHandlerRepository = chatWidgetContainerHandlerRepository;
}

void SingleWindowPluginObject::setPathsProvider(PathsProvider *pathsProvider)
{
	m_pathsProvider = pathsProvider;
}
void SingleWindowPluginObject::setSingleWindowChatWidgetContainerHandler(SingleWindowChatWidgetContainerHandler *singleWindowChatWidgetContainerHandler)
{
	m_singleWindowChatWidgetContainerHandler = singleWindowChatWidgetContainerHandler;
}

void SingleWindowPluginObject::setSingleWindowManager(SingleWindowManager *singleWindowManager)
{
	m_singleWindowManager = singleWindowManager;
}

void SingleWindowPluginObject::init()
{
	MainConfigurationWindow::registerUiFile(m_pathsProvider->dataPath() + QLatin1String("plugins/configuration/single_window.ui"));
	m_singleWindowChatWidgetContainerHandler.data()->setSingleWindow(m_singleWindowManager.data()->window());
	m_chatWidgetContainerHandlerRepository->registerChatWidgetContainerHandler(m_singleWindowChatWidgetContainerHandler);
}

void SingleWindowPluginObject::done()
{
	m_chatWidgetContainerHandlerRepository->unregisterChatWidgetContainerHandler(m_singleWindowChatWidgetContainerHandler);
	m_singleWindowChatWidgetContainerHandler.data()->setSingleWindow(nullptr);
	MainConfigurationWindow::unregisterUiFile(m_pathsProvider->dataPath() + QLatin1String("plugins/configuration/single_window.ui"));
}

#include "moc_single-window-plugin-object.cpp"
