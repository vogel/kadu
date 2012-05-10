/*
 * %kadu copyright begin%
 * Copyright 2012 Piotr Dąbrowski (ultr@ultr.pl)
 * Copyright 2012 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

template <typename T_dest, typename T_src>
class Adapter
{
public:
	virtual T_dest adapt(const T_src &) = 0;

};

#endif // ADAPTER_H
