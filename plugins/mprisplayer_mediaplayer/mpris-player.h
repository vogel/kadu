/*
 * %kadu copyright begin%
 * Copyright 2011 Sławomir Stępień (s.stepien@interia.pl)
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

#ifndef MPRIS_PLAYER_H
#define MPRIS_PLAYER_H

#include <QtCore/QMap>

#include "configuration/configuration-aware-object.h"
#include "misc/path-conversion.h"

#include "plugins/mediaplayer/mpris_mediaplayer.h"

class QString;

class MPRISPlayer : public MPRISMediaPlayer
{
	Q_OBJECT
	Q_DISABLE_COPY(MPRISPlayer)

	static MPRISPlayer *Instance;
	static const QString UserPlayersListFile;
	static const QString GlobalPlayersListFile;

	explicit MPRISPlayer();
	virtual ~MPRISPlayer();

	void prepareUserPlayersFile();
	void replacePlugin();
	void choosePlayer(const QString &key, const QString &value);

public:
	static void createInstance();
	static void destroyInstance();
	static MPRISPlayer *instance() { return Instance; }

	static const QString userPlayersListFileName() { return profilePath(UserPlayersListFile); }
	static const QString globalPlayersListFileName() { return dataPath(GlobalPlayersListFile); }

	void configurationApplied();

};

#endif /* MPRIS_PLAYER_H */
