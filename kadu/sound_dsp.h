#ifndef SOUND_H
#define SOUND_H

#include <qobject.h>

class SoundDsp : public QObject
{
	Q_OBJECT

	public:
		SoundDsp();
		virtual ~SoundDsp();

	public slots:
		void setup();
		void free();
		void playSample(char *data, int length);
		void recordSample(char *data, int length);

	private:
		int fd;
};

extern SoundDsp *sound_dsp;

/*class DspSoundDevice : public SoundDevice
{
	Q_OBJECT
	public:
		DspSoundDevice(const int freq, const int bits, const int chans, QObject *parent = 0, const char *name = 0);
		~DspSoundDevice();

	private:
		int fd;

	protected:
		void doPlaying();
		void doRecording();

	friend class PlayThread;
	friend class RecordThread;
};*/

#endif
