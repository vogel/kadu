#ifndef PLAYER_EXTERNAL_H
#define PLAYER_EXTERNAL_H

#include <QtCore/QObject>
#include <QtCore/QString>

#include <phonon/mediaobject.h>
#include <phonon/audiooutput.h>
#include <phonon/phononnamespace.h>

#include "modules/sound/sound-player.h"

/**
 * @defgroup phonon_sound Phonon sound
 * @{
 */
class PhononPlayer : public SoundPlayer
{
	Q_OBJECT

	Phonon::MediaObject *music;
	Phonon::AudioOutput *output;

public:
	PhononPlayer(QObject *parent = 0);
	virtual ~PhononPlayer();

	virtual bool isSimplePlayer() { return true; }

public slots:
	virtual void playSound(const QString &path, bool volumeControl, double volume);

	virtual SoundDevice openDevice(SoundDeviceType type, int sampleRate, int channels) { return 0; }
	virtual void closeDevice(SoundDevice device) {}

	virtual bool playSample(SoundDevice device, const qint16 *data, int length) { return false; }
	virtual bool recordSample(SoundDevice device, qint16 *data, int length) { return false; }

	virtual void setFlushingEnabled(SoundDevice device, bool enabled) {}

};

extern PhononPlayer *phonon_player;

/** @} */

#endif
