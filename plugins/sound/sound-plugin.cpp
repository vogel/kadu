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

#include "configuration/gui/sound-configuration-ui-handler.h"
#include "notify/sound-notifier.h"
#include "sound-actions.h"
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

	m_configurationUiHandler = new SoundConfigurationUiHandler{this};
	m_configurationUiHandler->setManager(m_soundManager);
	m_configurationUiHandler->setSoundThemeManager(m_soundThemeManager);

	m_soundNotifier = new SoundNotifier{this};
	m_soundNotifier->setManager(m_soundManager);
	m_soundNotifier->setConfigurationUiHandler(m_configurationUiHandler);
	NotificationManager::instance()->registerNotifier(m_soundNotifier);

	MainConfigurationWindow::registerUiFile(Application::instance()->pathsProvider()->dataPath() + QLatin1String("plugins/configuration/sound.ui"));
	MainConfigurationWindow::registerUiHandler(m_configurationUiHandler);

	m_soundActions = new SoundActions{this};
	m_soundActions->setManager(m_soundManager);

	return true;
}

void SoundPlugin::done()
{
	if (m_soundActions)
		m_soundActions->deleteLater();

	if (m_configurationUiHandler)
	{
		MainConfigurationWindow::unregisterUiHandler(m_configurationUiHandler);
		m_configurationUiHandler->deleteLater();
		MainConfigurationWindow::unregisterUiFile(Application::instance()->pathsProvider()->dataPath() + QLatin1String("plugins/configuration/sound.ui"));
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
