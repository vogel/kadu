/*
 * %kadu copyright begin%
 * Copyright 2008, 2009, 2010, 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009, 2010, 2012 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2008, 2009 Tomasz Rostański (rozteck@interia.pl)
 * Copyright 2008, 2009 Michał Podsiadlik (michal@kadu.net)
 * Copyright 2005, 2006 Adrian Smarzewski (adrian@kadu.net)
 * Copyright 2005 Paweł Płuciennik (pawel_p@kadu.net)
 * Copyright 2007, 2008, 2009, 2010, 2011, 2012, 2013, 2014 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2010, 2011, 2013 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2006, 2007 Dawid Stawiarski (neeo@kadu.net)
 * Copyright 2005, 2006 Marcin Ślusarz (joi@kadu.net)
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

#include "configuration/configuration.h"
#include "configuration/deprecated-configuration-api.h"
#include "core/application.h"
#include "gui/actions/action-description.h"
#include "gui/actions/action.h"
#include "gui/menu/menu-inventory.h"
#include "debug.h"
#include "themes.h"

#include "sound-manager.h"
#include "sound-theme-manager.h"

#include "sound-actions.h"

SoundActions * SoundActions::Instance = 0;

void SoundActions::registerActions()
{
	if (Instance)
		return;

	Instance = new SoundActions();
}

void SoundActions::unregisterActions()
{
	delete Instance;
	Instance = 0;
}

SoundActions * SoundActions::instance()
{
	return Instance;
}

SoundActions::SoundActions()
{
	kdebugf();

	MuteActionDescription = new ActionDescription(this,
		ActionDescription::TypeGlobal, "muteSoundsAction",
		this, SLOT(muteActionActivated(QAction *, bool)),
		KaduIcon("audio-volume-high"), tr("Play Sounds"), true
	);
	connect(MuteActionDescription, SIGNAL(actionCreated(Action *)), this, SLOT(setMuteActionState()));

	MenuInventory::instance()
		->menu("main")
		->addAction(MuteActionDescription, KaduMenu::SectionMiscTools, 7)
		->update();

	setMuteActionState();

	kdebugf2();
}

SoundActions::~SoundActions()
{
	MenuInventory::instance()
		->menu("main")
		->removeAction(MuteActionDescription)
		->update();
}

void SoundActions::setMuteActionState()
{
	foreach (Action *action, MuteActionDescription->actions())
		action->setChecked(!SoundManager::instance()->isMuted());
}

void SoundActions::muteActionActivated(QAction  *action, bool toggled)
{
	Q_UNUSED(action)

	SoundManager::instance()->setMute(!toggled);
	setMuteActionState();

	Application::instance()->configuration()->deprecatedApi()->writeEntry("Sounds", "PlaySound", toggled);
}

void SoundActions::configurationUpdated()
{
	SoundManager::instance()->setMute(!Application::instance()->configuration()->deprecatedApi()->readBoolEntry("Sounds", "PlaySound"));
	setMuteActionState();
}

#include "moc_sound-actions.cpp"
