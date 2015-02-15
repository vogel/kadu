/*
 * Copyright 2010 Michał Obrembski (byku@byku.com.pl)
 * %kadu copyright begin%
 * Copyright 2011, 2012, 2013, 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "image-link-configuration.h"

ImageLinkConfiguration::ImageLinkConfiguration() :
		ShowImages(true), ShowVideos(true)
{
}

ImageLinkConfiguration::ImageLinkConfiguration(const ImageLinkConfiguration &copyMe)
{
	ShowImages = copyMe.ShowImages;
	ShowVideos = copyMe.ShowVideos;
}

ImageLinkConfiguration::~ImageLinkConfiguration()
{
}

ImageLinkConfiguration & ImageLinkConfiguration::operator = (const ImageLinkConfiguration &copyMe)
{
	ShowImages = copyMe.ShowImages;
	ShowVideos = copyMe.ShowVideos;

	return *this;
}

void ImageLinkConfiguration::setShowImages(bool showImages)
{
	ShowImages = showImages;
}

bool ImageLinkConfiguration::showImages() const
{
	return ShowImages;
}

void ImageLinkConfiguration::setShowVideos(bool showVideos)
{
	ShowVideos = showVideos;
}

bool ImageLinkConfiguration::showVideos() const
{
	return ShowVideos;
}
