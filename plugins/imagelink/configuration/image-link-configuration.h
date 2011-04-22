/*
 * Copyright 2010 Michał Obrembski (byku@byku.com.pl)
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

#ifndef IMAGE_LINK_CONFIGURATION_H
#define IMAGE_LINK_CONFIGURATION_H

#include "configuration/configuration-aware-object.h"

class ImageLinkConfiguration : public ConfigurationAwareObject
{
	bool ShowVideos;
	bool AutoStartVideos;
	bool ShowImages;

	void createDefaultConfiguration();

protected:
	virtual void configurationUpdated();

public:
	ImageLinkConfiguration();
	~ImageLinkConfiguration();

	bool showVideos() { return ShowVideos; }
	bool autoStartVideos() { return AutoStartVideos; }
	bool showImages() { return ShowImages; }

};

#endif // IMAGE_LINK_CONFIGURATION_H
