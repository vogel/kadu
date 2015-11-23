/*
 * %kadu copyright begin%
 * Copyright 2011, 2013 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2011, 2013, 2014, 2015 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "sound-plugin.h"

#include "gui/sound-actions.h"
#include "gui/sound-buddy-configuration-widget-factory.h"
#include "gui/sound-chat-configuration-widget-factory.h"
#include "gui/sound-configuration-ui-handler.h"
#include "notification/sound-notifier.h"
#include "sound-manager.h"
#include "sound-module.h"
#include "sound-theme-manager.h"

#include "core/application.h"
#include "core/core.h"
#include "gui/widgets/buddy-configuration-widget-factory-repository.h"
#include "gui/widgets/chat-configuration-widget-factory-repository.h"
#include "misc/memory.h"
#include "misc/paths-provider.h"

#include "notification/notification-manager.h"

QPointer<SoundManager> SoundPlugin::m_staticSoundManager;

SoundPlugin::SoundPlugin(QObject *parent) :
		QObject{parent}
{
}

SoundPlugin::~SoundPlugin()
{
}

SoundManager * SoundPlugin::soundManager()
{
	return m_staticSoundManager;
}

bool SoundPlugin::init(bool firstLoad)
{
	Q_UNUSED(firstLoad)

	auto modules = std::vector<std::unique_ptr<injeqt::module>>{};
	modules.emplace_back(make_unique<SoundModule>());

	m_injector = make_unique<injeqt::injector>(std::move(modules));
	static_cast<void>(m_injector->get<SoundActions>()); // register actions
	m_staticSoundManager = m_injector->get<SoundManager>();

	Core::instance()->buddyConfigurationWidgetFactoryRepository()->registerFactory(m_injector->get<SoundBuddyConfigurationWidgetFactory>());
	Core::instance()->chatConfigurationWidgetFactoryRepository()->registerFactory(m_injector->get<SoundChatConfigurationWidgetFactory>());
	Core::instance()->notificationManager()->registerNotifier(m_injector->get<SoundNotifier>());

	MainConfigurationWindow::registerUiFile(Application::instance()->pathsProvider()->dataPath() + QLatin1String{"plugins/configuration/sound.ui"});
	MainConfigurationWindow::registerUiHandler(m_injector->get<SoundConfigurationUiHandler>());

	return true;
}

void SoundPlugin::done()
{
	MainConfigurationWindow::unregisterUiHandler(m_injector->get<SoundConfigurationUiHandler>());
	MainConfigurationWindow::unregisterUiFile(Application::instance()->pathsProvider()->dataPath() + QLatin1String{"plugins/configuration/sound.ui"});

	if (Core::instance()) // TODO: hack
	{
		Core::instance()->notificationManager()->unregisterNotifier(m_injector->get<SoundNotifier>());
		Core::instance()->chatConfigurationWidgetFactoryRepository()->unregisterFactory(m_injector->get<SoundChatConfigurationWidgetFactory>());
		Core::instance()->buddyConfigurationWidgetFactoryRepository()->unregisterFactory(m_injector->get<SoundBuddyConfigurationWidgetFactory>());
	}

	m_injector.reset();
}

Q_EXPORT_PLUGIN2(sound, SoundPlugin)

#include "moc_sound-plugin.cpp"
