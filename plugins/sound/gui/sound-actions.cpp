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

#include "sound-actions.h"

#include "gui/sound-mute-action.h"
#include "sound-manager.h"

#include "gui/menu/menu-inventory.h"

SoundActions::SoundActions(QObject *parent) :
		QObject{parent}
{
	m_soundMuteAction = new SoundMuteAction{this};

	MenuInventory::instance()
		->menu("main")
		->addAction(m_soundMuteAction, KaduMenu::SectionMiscTools, 7)
		->update();
}

SoundActions::~SoundActions()
{
	MenuInventory::instance()
		->menu("main")
		->removeAction(m_soundMuteAction)
		->update();
}

void SoundActions::setSoundManager(SoundManager *soundManager)
{
	m_soundManager = soundManager;
	m_soundMuteAction->setSoundManager(m_soundManager);
	m_soundMuteAction->updateActionStates();
}

void SoundActions::configurationUpdated()
{
	m_soundMuteAction->updateActionStates();
}

#include "moc_sound-actions.cpp"
