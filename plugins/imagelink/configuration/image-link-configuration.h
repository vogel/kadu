/*
 * Copyright 2010 Michał Obrembski (byku@byku.com.pl)
 * %kadu copyright begin%
 * Copyright 2009, 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2008, 2011, 2012, 2013 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#ifndef IMAGE_LINK_CONFIGURATION_H
#define IMAGE_LINK_CONFIGURATION_H

class ImageLinkConfiguration
{
	bool ShowImages;
	bool ShowVideos;

public:
	ImageLinkConfiguration();
	ImageLinkConfiguration(const ImageLinkConfiguration &copyMe);
	~ImageLinkConfiguration();

	ImageLinkConfiguration & operator = (const ImageLinkConfiguration &copyMe);

	void setShowImages(bool showImages);
	bool showImages() const;

	void setShowVideos(bool showVideos);
	bool showVideos() const;

};

#endif // IMAGE_LINK_CONFIGURATION_H
