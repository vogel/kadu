#ifndef VOICE_DSP_H
#define VOICE_DSP_H

#include <qobject.h>

class VoiceDsp : public QObject
{
	Q_OBJECT

	public:
		VoiceDsp();
		virtual ~VoiceDsp();

	public slots:
		void setup();
		void free();
		void playSample(char *data, int length);
		void recordSample(char *data, int length);

	private:
		int fd;
};

extern VoiceDsp *voice_dsp;

#endif
