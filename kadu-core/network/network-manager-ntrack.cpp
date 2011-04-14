/*
 * %kadu copyright begin%
 * Copyright 2011 Piotr Dąbrowski (ultr@ultr.pl)
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

#include "network-manager-ntrack.h"

NetworkManagerNTrack::NetworkManagerNTrack()
{
	connect(QNtrack::instance(), SIGNAL(stateChanged(QNTrackState,QNTrackState)), this, SLOT(stateChanged(QNTrackState,QNTrackState)));
}

NetworkManagerNTrack::~NetworkManagerNTrack()
{
}

bool NetworkManagerNTrack::isOnline()
{
	QNTrackState state = QNtrack::instance()->networkState();
	return state == NTRACK_STATE_ONLINE || state == NTRACK_STATE_UNKNOWN;
}

void NetworkManagerNTrack::stateChanged(QNTrackState oldState, QNTrackState newState)
{
	bool wasOnline = oldState == NTRACK_STATE_ONLINE || oldState == NTRACK_STATE_UNKNOWN;
	bool isOnline = newState == NTRACK_STATE_ONLINE || newState == NTRACK_STATE_UNKNOWN;

	if (wasOnline != isOnline)
		onlineStateChanged(isOnline);
}
