#ifndef KADU_ESD_SOUND_H
#define KADU_ESD_SOUND_H
#include <qobject.h>
#include <qstring.h>
#include "../sound/sound.h"

class ESDPlayer : public QObject
{
	Q_OBJECT
	public:
		ESDPlayer(QObject *parent=0, const char *name=0);
		~ESDPlayer();
	private slots:
		void openDevice(SoundDeviceType type, int sample_rate, int channels, SoundDevice& device);
		void closeDevice(SoundDevice device);
		void playSample(SoundDevice device, const int16_t* data, int length, bool& result);
		void recordSample(SoundDevice device, int16_t* data, int length, bool& result);
		void setFlushingEnabled(SoundDevice device, bool enabled);
};

extern ESDPlayer *esd_player;

#endif

