#ifndef VOICE_DSP_H
#define VOICE_DSP_H

#include <qobject.h>

class VoiceDsp : public QObject
{
	Q_OBJECT

	public:
		VoiceDsp(QObject *parent=0, const char *name=0);
		virtual ~VoiceDsp();

	public slots:
		void playSample(char *data, int length);
		void recordSample(char *data, int length);
		void setup();
		void free();

	private:
		int fd;
		int tmp;
};

extern VoiceDsp* voice_dsp;

#endif
