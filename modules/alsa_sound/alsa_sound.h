#ifndef KADU_ALSA_SOUND_H
#define KADU_ALSA_SOUND_H
#include <qobject.h>
#include <qstring.h>
#include <qstringlist.h>

#define ALSA_PCM_NEW_HW_PARAMS_API
#define ALSA_PCM_NEW_SW_PARAMS_API
#include <asoundlib.h>

#include "../sound/sound.h"

class ALSAPlayerSlots : public QObject
{
	Q_OBJECT
	public:
		ALSAPlayerSlots(QObject *parent = 0, const char *name = 0);
		~ALSAPlayerSlots();
		bool isOk();

		static snd_pcm_t *alsa_open (const char *dev, int channels, int samplerate, bool play = true);

	public slots:
		void openDevice(SoundDeviceType type, int sample_rate, int channels, SoundDevice& device);
		void closeDevice(SoundDevice device);
		void playSample(SoundDevice device, const int16_t* data, int length, bool& result);
		void recordSample(SoundDevice device, int16_t* data, int length, bool& result);
		void setFlushingEnabled(SoundDevice device, bool enabled);
};

extern ALSAPlayerSlots *alsa_player_slots;

#endif
