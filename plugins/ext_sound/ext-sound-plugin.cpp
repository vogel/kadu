/*
 * %kadu copyright begin%
 * Copyright 2012, 2013 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#include "ext-sound-plugin.h"

#include "external-player.h"

#include "plugins/sound/sound-manager.h"
#include "plugins/sound/sound-plugin.h"

#include "core/application.h"
#include "gui/windows/main-configuration-window.h"
#include "misc/paths-provider.h"

ExtSoundPlugin::ExtSoundPlugin(QObject *parent) :
		QObject{parent}
{
}

ExtSoundPlugin::~ExtSoundPlugin()
{
}

bool ExtSoundPlugin::init(bool firstLoad)
{
	Q_UNUSED(firstLoad)

	m_externalPlayer = new ExternalPlayer{this};
	SoundPlugin::soundManager()->setPlayer(m_externalPlayer);
	MainConfigurationWindow::registerUiFile(Application::instance()->pathsProvider()->dataPath() + QLatin1String{"plugins/configuration/ext_sound.ui"});

	return true;
}

void ExtSoundPlugin::done()
{
	MainConfigurationWindow::unregisterUiFile(Application::instance()->pathsProvider()->dataPath() + QLatin1String{"plugins/configuration/ext_sound.ui"});
	SoundPlugin::soundManager()->setPlayer(nullptr);

	if (m_externalPlayer)
		m_externalPlayer->deleteLater();
}

Q_EXPORT_PLUGIN2(ext_sound, ExtSoundPlugin)

#include "moc_ext-sound-plugin.cpp"
