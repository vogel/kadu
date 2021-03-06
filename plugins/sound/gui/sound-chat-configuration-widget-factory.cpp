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

#include "sound-chat-configuration-widget-factory.h"

#include "gui/sound-chat-configuration-widget.h"
#include "plugin/plugin-injected-factory.h"
#include "sound-manager.h"

SoundChatConfigurationWidgetFactory::SoundChatConfigurationWidgetFactory(QObject *parent) : QObject{parent}
{
}

SoundChatConfigurationWidgetFactory::~SoundChatConfigurationWidgetFactory()
{
}

void SoundChatConfigurationWidgetFactory::setPluginInjectedFactory(PluginInjectedFactory *pluginInjectedFactory)
{
    m_pluginInjectedFactory = pluginInjectedFactory;
}

void SoundChatConfigurationWidgetFactory::setSoundManager(SoundManager *soundManager)
{
    m_soundManager = soundManager;
}

ChatConfigurationWidget *SoundChatConfigurationWidgetFactory::createWidget(const Chat &chat, QWidget *parent)
{
    return m_pluginInjectedFactory->makeInjected<SoundChatConfigurationWidget>(chat, m_soundManager, parent);
}
