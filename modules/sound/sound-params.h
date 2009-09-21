/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef SOUND_PARAMS_H
#define SOUND_PARAMS_H

#include <QtCore/QString>

class SoundParams
{
	
	QString FileName;
	bool VolumeControl;
	float Volume;

public:
	SoundParams(QString fileName = QString::null, bool volumeControl = false, float volume = 1);
	SoundParams(const SoundParams &copy);

	QString fileName() const { return FileName; }
	bool volumeControl() const { return VolumeControl; }
	float volume() const { return Volume; }

};

#endif // SOUND_PARAMS_H
