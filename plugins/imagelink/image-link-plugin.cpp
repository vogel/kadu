/*
 * %kadu copyright begin%
 * Copyright 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2010 Michał Obrembski (byku@byku.com.pl)
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

#include "gui/windows/main-configuration-window.h"
#include "misc/path-conversion.h"

#include "image-link.h"

#include "image-link-plugin.h"

ImageLinkPlugin::~ImageLinkPlugin()
{
}

int ImageLinkPlugin::init(bool firstLoad)
{
	Q_UNUSED(firstLoad)

	ImageLink::createInstance();
	MainConfigurationWindow::registerUiFile(dataPath("plugins/configuration/image-link.ui"));

	return 0;
}

void ImageLinkPlugin::done()
{
	MainConfigurationWindow::unregisterUiFile(dataPath("plugins/configuration/image-link.ui"));
	ImageLink::destroyInstance();
}

Q_EXPORT_PLUGIN2(imagelink, ImageLinkPlugin)
