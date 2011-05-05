/*
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

#ifndef HINTS_PLUGIN_H
#define HINTS_PLUGIN_H

#include <QtCore/QObject>

#include "plugins/generic-plugin.h"

class HintManager;

class HintsPlugin : public QObject, public GenericPlugin
{
	Q_OBJECT
	Q_INTERFACES(GenericPlugin)

	static HintsPlugin * Instance;

	HintManager *HintManagerInstance;

public:
	static HintsPlugin * instance() { return Instance; }

	explicit HintsPlugin(QObject *parent = 0);
	virtual ~HintsPlugin();

	virtual int init(bool firstLoad);
	virtual void done();

	HintManager * hintsManger() { return HintManagerInstance; }

};

#endif // HINTS_PLUGIN_H
