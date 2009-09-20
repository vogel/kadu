/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef SOUND_EVENT_H
#define SOUND_EVENT_H

#include <QtCore/QEvent>

#include "sound.h"

class SoundEvent : public QEvent
{
	SoundDevice Data;
	static int EventNumber;

public:
	SoundEvent(SoundDevice data) : QEvent((QEvent::Type)EventNumber) { Data = data; }
	SoundDevice data() { return Data; }

	static int eventNumber() { return EventNumber; }

};

#endif // SOUND_EVENT_H
