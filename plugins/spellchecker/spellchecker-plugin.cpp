/*
 * %kadu copyright begin%
 * Copyright 2008, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2011 Sławomir Stępień (s.stepien@interia.pl)
 * Copyright 2007, 2008, 2009, 2010, 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2011 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#include "misc/path-conversion.h"

#include "configuration/spellchecker-configuration.h"
#include "spellchecker-plugin.h"
#include "spellchecker.h"
#include "suggester.h"

SpellCheckerPlugin * SpellCheckerPlugin::Instance = 0;

SpellCheckerPlugin::SpellCheckerPlugin(QObject *parent) :
		QObject(parent), SpellCheckerInstance(0)
{
	Instance = this;
}

SpellCheckerPlugin::~SpellCheckerPlugin()
{
	Instance = 0;
}

int SpellCheckerPlugin::init(bool firstLoad)
{
	Q_UNUSED(firstLoad)

	SpellCheckerInstance = new SpellChecker(this);
	SpellcheckerConfiguration::createInstance();

	MainConfigurationWindow::registerUiFile(dataPath("kadu/plugins/configuration/spellchecker.ui"));
	MainConfigurationWindow::registerUiHandler(SpellCheckerInstance);

	return 0;
}

void SpellCheckerPlugin::done()
{
	Suggester::destroyInstance();
	SpellcheckerConfiguration::destroyInstance();

	MainConfigurationWindow::unregisterUiHandler(SpellCheckerInstance);
	MainConfigurationWindow::unregisterUiFile(dataPath("kadu/plugins/configuration/spellchecker.ui"));

	delete SpellCheckerInstance;
	SpellCheckerInstance = 0;
}

Q_EXPORT_PLUGIN2(spellchecker, SpellCheckerPlugin)
