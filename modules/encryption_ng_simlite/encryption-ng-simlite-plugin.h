/*
 * %kadu copyright begin%
 * %kadu copyright end%
 * Copyright 2010, 2011 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#ifndef ENCRYPTION_NG_SIMLITE_PLUGIN_H
#define ENCRYPTION_NG_SIMLITE_PLUGIN_H

#include <QtCore/QObject>

#include "plugins/generic-plugin.h"

class EngryptionNgSimlitePlugin : public QObject, public GenericPlugin
{
	Q_OBJECT
	Q_INTERFACES(GenericPlugin)

public:
	virtual ~EngryptionNgSimlitePlugin();

	virtual int init(bool firstLoad);
	virtual void done();

};

#endif // ENCRYPTION_NG_SIMLITE_PLUGIN_H
