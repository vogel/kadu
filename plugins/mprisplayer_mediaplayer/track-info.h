/*
 * %kadu copyright begin%
 * Copyright 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#ifndef TRACK_INFO_H
#define TRACK_INFO_H

#include <QtCore/QString>

class TrackInfo
{
	QString Title;
	QString Artist;
	QString Album;
	uint TrackNumber;
	QString File;
	uint Length;

public:
	TrackInfo();
	~TrackInfo();

	TrackInfo(const TrackInfo &copyMe);

	TrackInfo & operator = (const TrackInfo &copyMe);

	void setTitle(const QString &title);
	QString title() const;

	void setArtist(const QString &artist);
	QString artist() const;

	void setAlbum(const QString &album);
	QString album() const;

	void setTrackNumber(uint trackNumber);
	uint trackNumber() const;

	void setFile(const QString &file);
	QString file() const;

	void setLength(uint length);
	uint length() const;

};

#endif // TRACK_INFO_H
