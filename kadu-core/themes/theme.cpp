/*
 * %kadu copyright begin%
 * Copyright 2012 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2010, 2011, 2012, 2013, 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "theme.h"

Theme Theme::null("", "");

Theme::Theme(const QString &path, const QString &name) :
		Path(path), Name(name)
{
}

Theme::Theme(const Theme &copyMe)
{
	Path = copyMe.Path;
	Name = copyMe.Name;
}

Theme & Theme::operator = (const Theme &copyMe)
{
	Path = copyMe.Path;
	Name = copyMe.Name;

	return *this;
}

bool Theme::operator == (const Theme &compareTo)
{
	return Path == compareTo.Path && Name == compareTo.Name;
}

bool Theme::operator != (const Theme &compareTo)
{
	return !(*this == compareTo);
}

bool Theme::isValid() const
{
	return !Path.isEmpty() && !Name.isEmpty();
}

Theme::operator bool () const
{
	return isValid();
}
