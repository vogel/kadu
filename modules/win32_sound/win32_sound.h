#ifndef KADU_WIN32_SOUND_H
#define KADU_WIN32_SOUND_H
#include <QtCore/QObject>
#include <QtCore/QString>
#include <QtCore/QStringList>
#include "../sound/sound.h"

/**
 * @defgroup dsp_sound DSP sound
 * @{
 */
class WIN32PlayerSlots : public ConfigurationUiHandler
{
	Q_OBJECT

	void createDefaultConfiguration();
	QStringList inNames;
	QStringList inValues;
	QStringList outNames;
	QStringList outValues;

public slots:
	void openDevice(SoundDeviceType type, int sample_rate, int channels, SoundDevice* device);
	void closeDevice(SoundDevice device);
	void playSample(SoundDevice device, const qint16* data, int length, bool* result);

protected:
	void mainConfigurationWindowCreated(MainConfigurationWindow *mainConfigurationWindow);

public:
	WIN32PlayerSlots();
	~WIN32PlayerSlots();

};

extern WIN32PlayerSlots *win32_player_slots;

/** @} */

#endif
