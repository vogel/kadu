#ifndef KADU_DSP_SOUND_H
#define KADU_DSP_SOUND_H
#include <qobject.h>
#include <qstring.h>
#include <qstringlist.h>
#include "../sound/sound.h"

/**
 * @defgroup dsp_sound DSP sound
 * @{
 */
class OSSPlayerSlots : public QObject
{
	Q_OBJECT

	static int write_all(int fd, const char *data, int length, int chunksize);
	static int read_all(int fd, char *buffer, int count);

	void createDefaultConfiguration();

public slots:
	void openDevice(SoundDeviceType type, int sample_rate, int channels, SoundDevice& device);
	void closeDevice(SoundDevice device);
	void playSample(SoundDevice device, const int16_t* data, int length, bool& result);
	void recordSample(SoundDevice device, int16_t* data, int length, bool& result);		
	void setFlushingEnabled(SoundDevice device, bool enabled);

public:
	OSSPlayerSlots(QObject *parent=0, const char *name=0);
	~OSSPlayerSlots();

};

extern OSSPlayerSlots *oss_player_slots;

/** @} */

#endif
