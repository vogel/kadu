/*
 * %kadu copyright begin%
 * Copyright 2012, 2013 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2011, 2012 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#include <QtGui/QIcon>

#include "icons/icons-manager.h"
#include "misc/paths-provider.h"

#include "kadu-icon.h"

QString KaduIcon::fullPath() const
{
	return IconsManager::instance()->iconPath(*this);
}

QString KaduIcon::webKitPath() const
{
	return PathsProvider::webKitPath(fullPath());
}

QIcon KaduIcon::icon() const
{
	return IconsManager::instance()->iconByPath(themePath(), path());
}
