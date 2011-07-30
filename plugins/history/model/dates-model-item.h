/*
 * %kadu copyright begin%
 * Copyright 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
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

#ifndef DATES_MODELI_TEM_H
#define DATES_MODELI_TEM_H

#include <QtCore/QDate>

struct DatesModelItem
{
	QDate Date;
	int Count;
	QString Title;

	DatesModelItem(const QDate &date, int count, const QString &title)
		: Date(date), Count(count), Title(title)
	{
	}
};

#endif // DATES_MODEL_ITEM_H
