/*
 * %kadu copyright begin%
 * Copyright 2011 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "track-info.h"

TrackInfo::TrackInfo() :
		TrackNumber(0), Length(0)
{
}

TrackInfo::~TrackInfo()
{
}

TrackInfo::TrackInfo(const TrackInfo &copyMe)
{
	Title = copyMe.Title;
	Artist = copyMe.Artist;
	Album = copyMe.Album;
	TrackNumber = copyMe.TrackNumber;
	File = copyMe.File;
	Length = copyMe.Length;
}

TrackInfo & TrackInfo::operator = (const TrackInfo &copyMe)
{
	Title = copyMe.Title;
	Artist = copyMe.Artist;
	Album = copyMe.Album;
	TrackNumber = copyMe.TrackNumber;
	File = copyMe.File;
	Length = copyMe.Length;

	return *this;
}

void TrackInfo::setTitle(const QString &title)
{
	Title = title;
}

QString TrackInfo::title() const
{
	return Title;
}

void TrackInfo::setArtist(const QString &artist)
{
	Artist = artist;
}

QString TrackInfo::artist() const
{
	return Artist;
}

void TrackInfo::setAlbum(const QString &album)
{
	Album = album;
}

QString TrackInfo::album() const
{
	return Album;
}

void TrackInfo::setTrackNumber(uint trackNumber)
{
	TrackNumber = trackNumber;
}

uint TrackInfo::trackNumber() const
{
	return TrackNumber;
}

void TrackInfo::setFile(const QString &file)
{
	File = file;
}

QString TrackInfo::file() const
{
	return File;
}

void TrackInfo::setLength(uint length)
{
	Length = length;
}

uint TrackInfo::length() const
{
	return Length;
}
