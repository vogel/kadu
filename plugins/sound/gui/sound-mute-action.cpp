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

#include "sound-mute-action.h"

#include "sound-manager.h"

#include "configuration/configuration.h"
#include "configuration/deprecated-configuration-api.h"
#include "core/application.h"
#include "gui/actions/action.h"

SoundMuteAction::SoundMuteAction(QObject *parent) :
		// using C++ initializers breaks Qt's lupdate
		ActionDescription(parent)
{
	setCheckable(true);
	setIcon(KaduIcon("audio-volume-high"));
	setName("muteSoundsAction");
	setText(tr("Play Sounds"));
	setType(ActionDescription::TypeGlobal);

	registerAction();
}

SoundMuteAction::~SoundMuteAction()
{
}

void SoundMuteAction::setSoundManager(SoundManager *soundManager)
{
	m_soundManager = soundManager;
}

void SoundMuteAction::actionTriggered(QAction *sender, bool toggled)
{
	Q_UNUSED(sender);

	m_soundManager->setMute(!toggled);
	updateActionStates();

	Application::instance()->configuration()->deprecatedApi()->writeEntry("Sounds", "PlaySound", toggled);
}

void SoundMuteAction::updateActionState(Action *action)
{
	if (m_soundManager)
		action->setChecked(!m_soundManager->isMuted());
}
