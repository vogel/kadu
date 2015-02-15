/*
 * %kadu copyright begin%
 * Copyright 2011, 2014 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2011, 2013, 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#ifndef SPELLCHECKER_PLUGIN_H
#define SPELLCHECKER_PLUGIN_H

#include <QtCore/QObject>

#include "plugin/plugin-root-component.h"

class SpellChecker;

class SpellCheckerPlugin : public QObject, PluginRootComponent
{
	Q_OBJECT
	Q_INTERFACES(PluginRootComponent)
	Q_PLUGIN_METADATA(IID "im.kadu.PluginRootComponent")

	static SpellCheckerPlugin *Instance;

	SpellChecker *SpellCheckerInstance;

public:
	static SpellCheckerPlugin * instance() { return Instance; }

	explicit SpellCheckerPlugin(QObject *parent = 0);
	virtual ~SpellCheckerPlugin();

	virtual bool init(bool firstLoad);
	virtual void done();

	SpellChecker * spellChecker() const { return SpellCheckerInstance; }

};

#endif // SPELLCHECKER_PLUGIN_H
