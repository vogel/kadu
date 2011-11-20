/*
 * %kadu copyright begin%
 * Copyright 2008, 2009, 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2008, 2009 Tomasz Rostański (rozteck@interia.pl)
 * Copyright 2009 Michał Podsiadlik (michal@kadu.net)
 * Copyright 2005 Adrian Smarzewski (adrian@kadu.net)
 * Copyright 2005 Paweł Płuciennik (pawel_p@kadu.net)
 * Copyright 2007, 2008, 2009, 2010, 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2006 Marcin Ślusarz (joi@kadu.net)
 * Copyright 2006, 2007 Dawid Stawiarski (neeo@kadu.net)
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

#ifndef SOUND_ACTIONS_H
#define SOUND_ACTIONS_H

#include <QtCore/QObject>

#include "configuration/configuration-aware-object.h"

class QAction;

class ActionDescription;

class SoundActions : public QObject, public ConfigurationAwareObject
{
	Q_OBJECT
	Q_DISABLE_COPY(SoundActions)

	static SoundActions *Instance;

	ActionDescription *MuteActionDescription;

	SoundActions();
	virtual ~SoundActions();

private slots:
	void setMuteActionState();
	void muteActionActivated(QAction *action, bool is_on);

protected:
	virtual void configurationUpdated();

public:
	static void registerActions();
	static void unregisterActions();

	static SoundActions * instance();

};

#endif // SOUND_ACTIONS_H
