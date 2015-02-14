/*
 * %kadu copyright begin%
 * Copyright 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2009 Bartłomiej Zimoń (uzi18@o2.pl)
 * Copyright 2004 Adrian Smarzewski (adrian@kadu.net)
 * Copyright 2007, 2008, 2009, 2010, 2011, 2013 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2011, 2013 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2004, 2006 Marcin Ślusarz (joi@kadu.net)
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

#include "gui/sound-configuration-ui-handler.h"
#include "gui/sound-actions.h"
#include "notify/sound-notifier.h"
#include "sound-manager.h"
#include "sound-theme-manager.h"

#include "core/application.h"
#include "misc/paths-provider.h"

#include "notify/notification-manager.h"

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

	m_soundThemeManager = new SoundThemeManager{this};

	m_soundManager = new SoundManager{this};
	m_soundManager->setSoundThemeManager(m_soundThemeManager);
	m_staticSoundManager = m_soundManager;

	m_soundConfigurationUiHandler = new SoundConfigurationUiHandler{this};
	m_soundConfigurationUiHandler->setSoundManager(m_soundManager);
	m_soundConfigurationUiHandler->setSoundThemeManager(m_soundThemeManager);

	m_soundNotifier = new SoundNotifier{this};
	m_soundNotifier->setSoundManager(m_soundManager);
	m_soundNotifier->setSoundConfigurationUiHandler(m_soundConfigurationUiHandler);
	NotificationManager::instance()->registerNotifier(m_soundNotifier);

	MainConfigurationWindow::registerUiFile(Application::instance()->pathsProvider()->dataPath() + QLatin1String("plugins/configuration/sound.ui"));
	MainConfigurationWindow::registerUiHandler(m_soundConfigurationUiHandler);

	m_soundActions = new SoundActions{this};
	m_soundActions->setSoundManager(m_soundManager);

	return true;
}

void SoundPlugin::done()
{
	if (m_soundActions)
		m_soundActions->deleteLater();

	if (m_soundConfigurationUiHandler)
	{
		MainConfigurationWindow::unregisterUiHandler(m_soundConfigurationUiHandler);
		MainConfigurationWindow::unregisterUiFile(Application::instance()->pathsProvider()->dataPath() + QLatin1String("plugins/configuration/sound.ui"));
		m_soundConfigurationUiHandler->deleteLater();
	}

	if (m_soundNotifier)
	{
		NotificationManager::instance()->unregisterNotifier(m_soundNotifier);
		m_soundNotifier->deleteLater();
	}

	if (m_soundManager)
		m_soundManager->deleteLater();

	if (m_soundThemeManager)
		m_soundThemeManager->deleteLater();
}

Q_EXPORT_PLUGIN2(sound, SoundPlugin)

#include "moc_sound-plugin.cpp"
