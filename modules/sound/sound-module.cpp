/*
 * %kadu copyright begin%
 * 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "sound.h"

extern "C" KADU_EXPORT int sound_init(bool firstLoad)
{
	kdebugf();

	new SoundManager(firstLoad, "sounds", "sound.conf");
	MainConfigurationWindow::registerUiFile(dataPath("kadu/modules/configuration/sound.ui"));
	MainConfigurationWindow::registerUiHandler(sound_manager);

	qRegisterMetaType<SoundDevice>("SoundDevice");
	qRegisterMetaType<SoundDeviceType>("SoundDeviceType");

	kdebugf2();
	return 0;
}

extern "C" KADU_EXPORT void sound_close()
{
	kdebugf();
	MainConfigurationWindow::unregisterUiFile(dataPath("kadu/modules/configuration/sound.ui"));
	MainConfigurationWindow::unregisterUiHandler(sound_manager);

	delete sound_manager;
	sound_manager = 0;
	kdebugf2();
}
