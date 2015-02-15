/*
 * %kadu copyright begin%
 * Copyright 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2010, 2012, 2013, 2014 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#ifndef THEME_H
#define THEME_H

#include <QtCore/QString>

#include "exports.h"

class KADUAPI Theme
{
	QString Path;
	QString Name;

public:
	static Theme null;

	Theme() {}
	Theme(const QString &path, const QString &name);
	Theme(const Theme &copyMe);

	Theme & operator = (const Theme &copyMe);
	bool operator == (const Theme &compareTo);
	bool operator != (const Theme &compareTo);

	const QString & path() const { return Path; }
	const QString & name() const { return Name; }

	bool isValid() const;
	explicit operator bool() const;

};

#endif // THEME_H
