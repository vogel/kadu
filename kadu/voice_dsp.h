#ifndef VOICE_DSP_H
#define VOICE_DSP_H

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

#endif
