/*
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

#ifndef GENERIC_PLUGIN_H
#define GENERIC_PLUGIN_H

#include <QtCore/QtPlugin>

class GenericPlugin
{
public:
	virtual ~GenericPlugin() {}

	virtual void init() = 0;
	virtual void done() = 0;

};

Q_DECLARE_INTERFACE(GenericPlugin, "im.kadu.GenericPlugin/0.1")

#endif // GENERIC_PLUGIN_H
