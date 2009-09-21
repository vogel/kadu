/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "sound-params.h"

SoundParams::SoundParams(QString fileName, bool volumeControl, float volume) :
		FileName(fileName), VolumeControl(volumeControl), Volume(volume)
{
}

SoundParams::SoundParams(const SoundParams &copy) :
		FileName(copy.FileName), VolumeControl(copy.VolumeControl), Volume(copy.Volume)
{
}
