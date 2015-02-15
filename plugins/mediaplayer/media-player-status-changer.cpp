/*
 * %kadu copyright begin%
 * Copyright 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "media-player-status-changer.h"

#include "status/status.h"

MediaPlayerStatusChanger::MediaPlayerStatusChanger(QObject *parent) :
		StatusChanger{900, parent},
		_disabled{true},
		_mediaPlayerStatusPosition{DescriptionReplace}
{
}

MediaPlayerStatusChanger::~MediaPlayerStatusChanger()
{
}

void MediaPlayerStatusChanger::changeStatus(StatusContainer *container, Status &status)
{
	Q_UNUSED(container)

	if (_disabled || status.isDisconnected())
		return;

	QString description = status.description();
	switch (_mediaPlayerStatusPosition)
	{
		case DescriptionReplace:
			description = _title;
			break;

		case DescriptionPrepend:
			description = _title + description;
			break;

		case DescriptionAppend:
			description = description + _title;
			break;

		case PlayerTagReplace:
			if (description.indexOf("%player%") > -1)
				description.replace("%player%", _title);
			break;
	}

	status.setDescription(description);
}

void MediaPlayerStatusChanger::setTitle(const QString &newTitle)
{
	_disabled = false;

	if (newTitle != _title)
	{
		_title = newTitle;
		emit statusChanged(0);
	}
}

void MediaPlayerStatusChanger::setDisable(bool disable)
{
	_disabled = disable;
	emit statusChanged(0);
}

bool MediaPlayerStatusChanger::isDisabled() const
{
	return _disabled;
}

void MediaPlayerStatusChanger::changePositionInStatus(ChangeDescriptionTo newSongInfoPlace)
{
	if (_mediaPlayerStatusPosition != newSongInfoPlace)
	{
		_mediaPlayerStatusPosition = newSongInfoPlace;
		if (!_disabled)
			emit statusChanged(0);
	}
}

MediaPlayerStatusChanger::ChangeDescriptionTo MediaPlayerStatusChanger::changeDescriptionTo() const
{
	return _mediaPlayerStatusPosition;
}

#include "moc_media-player-status-changer.cpp"
