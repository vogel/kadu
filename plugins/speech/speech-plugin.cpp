/*
 * %kadu copyright begin%
 * Copyright 2008, 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
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

#include "debug.h"
#include "exports.h"
#include "speech-configuration-ui-handler.h"
#include "speech.h"

#include "speech-plugin.h"

SpeechPlugin::~SpeechPlugin()
{
}

int SpeechPlugin::init(bool firstLoad)
{
	Q_UNUSED(firstLoad)

	kdebugf();

	Speech::createInstance();
	SpeechConfigurationUiHandler::registerUiHandler();

	kdebugf2();
	return 0;
}

void SpeechPlugin::done()
{
	kdebugf();

	SpeechConfigurationUiHandler::unregisterUiHandler();
	Speech::destroyInstance();

	kdebugf2();
}

Q_EXPORT_PLUGIN2(speech, SpeechPlugin)
