/*
 * %kadu copyright begin%
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2009, 2010 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2004, 2006 Marcin Ślusarz (joi@kadu.net)
 * Copyright 2010 Maciej Płaza (plaza.maciej@gmail.com)
 * Copyright 2007, 2008, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2008 Michał Podsiadlik (michal@kadu.net)
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

#include "hint_manager.h"
#include "hints-configuration-ui-handler.h"

#include "hints-plugin.h"

HintsPlugin * HintsPlugin::Instance = 0;

HintsPlugin::HintsPlugin(QObject *parent) :
		QObject(parent), HintManagerInstance(0)
{
	Instance = this;
}

HintsPlugin::~HintsPlugin()
{
	Instance = 0;
}

int HintsPlugin::init(bool firstLoad)
{
	Q_UNUSED(firstLoad)

	HintManagerInstance = new HintManager(this);
	MainConfigurationWindow::registerUiFile(dataPath("kadu/plugins/configuration/hints.ui"));
	MainConfigurationWindow::registerUiHandler(HintManagerInstance->uiHandler());

	return 0;
}

void HintsPlugin::done()
{
	MainConfigurationWindow::unregisterUiHandler(HintManagerInstance->uiHandler());
	MainConfigurationWindow::unregisterUiFile(dataPath("kadu/plugins/configuration/hints.ui"));
}

Q_EXPORT_PLUGIN2(hints, HintsPlugin)
