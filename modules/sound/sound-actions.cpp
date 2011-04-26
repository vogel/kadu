/*
 * %kadu copyright begin%
 * Copyright 2006, 2007 Dawid Stawiarski (neeo@kadu.net)
 * Copyright 2010 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2009, 2010 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2008, 2009, 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2005, 2006 Marcin Ślusarz (joi@kadu.net)
 * Copyright 2005, 2006 Adrian Smarzewski (adrian@kadu.net)
 * Copyright 2007, 2008, 2009, 2010, 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2008, 2009 Michał Podsiadlik (michal@kadu.net)
 * Copyright 2008, 2009 Tomasz Rostański (rozteck@interia.pl)
 * Copyright 2005 Paweł Płuciennik (pawel_p@kadu.net)
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

#include "configuration/configuration-file.h"
#include "core/core.h"
#include "gui/actions/action.h"
#include "gui/windows/kadu-window.h"
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

	Core::instance()->kaduWindow()->insertMenuActionDescription(MuteActionDescription, KaduWindow::MenuKadu, 7);

	setMuteActionState();

	kdebugf2();
}

SoundActions::~SoundActions()
{
	Core::instance()->kaduWindow()->removeMenuActionDescription(MuteActionDescription);
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

	config_file.writeEntry("Sounds", "PlaySound", toggled);
}

void SoundActions::configurationUpdated()
{
	SoundManager::instance()->setMute(!config_file.readBoolEntry("Sounds", "PlaySound"));
	setMuteActionState();
}
