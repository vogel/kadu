#ifndef VOICE_ARTS_H
#define VOICE_ARTS_H

#include <qobject.h>

extern "C" {
#include <kde/artsc/artsc.h>
};

class VoiceArts : public QObject
{
	Q_OBJECT

	public:
		VoiceArts();
		virtual ~VoiceArts();

	public slots:
		void setup();
		void free();
		void playSample(char *data, int length);
		void recordSample(char *data, int length);

	private:
		arts_stream_t playstream;
		arts_stream_t recstream;
};

extern VoiceArts *voice_arts;

#endif
