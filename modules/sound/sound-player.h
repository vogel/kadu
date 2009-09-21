/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef SOUND_PLAYER_H
#define SOUND_PLAYER_H

#include <QtCore/QObject>

#include "sound.h"

class SoundPlayer : public QObject
{
	Q_OBJECT

public:
	SoundPlayer(QObject *parent) : QObject(parent) {}
	virtual ~SoundPlayer() {}

	virtual bool isSimplePlayer() = 0;

public slots:
	virtual void playSound(const QString &sound, bool volumeControl, double vol) = 0;

	virtual SoundDevice openDevice(SoundDeviceType type, int sampleRate, int channels) = 0;
	virtual void closeDevice(SoundDevice device) = 0;

	virtual bool playSample(SoundDevice device, const qint16 *data, int length) = 0;
	virtual bool recordSample(SoundDevice device, qint16 *data, int length) = 0;

	virtual void setFlushingEnabled(SoundDevice device, bool enabled) = 0;

};

#endif // SOUND_PLAYER_H
