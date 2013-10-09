/*
 * %kadu copyright begin%
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2012 Piotr Dąbrowski (ultr@ultr.pl)
 * Copyright 2004 Adrian Smarzewski (adrian@kadu.net)
 * Copyright 2007, 2008, 2009, 2010, 2011, 2012 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2012 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2004, 2006 Marcin Ślusarz (joi@kadu.net)
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

#ifndef ADAPTER_H
#define ADAPTER_H

/**
 * @addtogroup Core
 * @{
 */

/**
 * @class Adapter
 * @short Interface for adapting one type to another.
 * @author Rafał 'Vogel' Malinowski
 * @author Piotr Dąbrowski
 * @param T_dest type of result class
 * @param T_src type of class to adapt
 */
template <typename T_dest, typename T_src>
class Adapter
{
public:
	virtual ~Adapter<T_dest, T_src>() {}

	/**
	 * @short Adapt instance of class from one type to another.
	 * @author Rafał 'Vogel' Malinowski
	 * @author Piotr Dąbrowski
	 * @param object object to adapt
	 * @return object adapted to type T_dest
	 */
	virtual T_dest adapt(const T_src &object) = 0;

};

/**
 * @}
 */

#endif // ADAPTER_H
