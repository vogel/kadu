#ifndef VOICE_H
#define VOICE_H

#include <qobject.h>
#include <qthread.h>
#include <qmutex.h>
#include <qsemaphore.h>
#include <qvaluelist.h>
#include <gsm.h>

class PlayThread;
class RecordThread;

struct gsm_sample {
	char *data;
	int length;
};

class VoiceManager : public QObject {
	Q_OBJECT

	public:
		VoiceManager();
		void setup();
		void free();
		void resetCodec();
		void addGsmSample(char *data, int length);

	signals:
		void setupSoundDevice();
		void freeSoundDevice();
		void gsmSampleRecorded(char *data, int length);
		void playSample(char *data, int length);
		void recordSample(char *data, int length);

	private slots:
		void playGsmSampleReceived(char *data, int length);
		void recordSampleReceived(char *data, int length);

	private:
		PlayThread pt;
		gsm voice_enc;
		RecordThread rt;
		gsm voice_dec;

	friend class PlayThread;
	friend class RecordThread;
};

class PlayThread : public QThread, public QObject
{
	Q_OBJECT

	public:
		PlayThread();
		void run();

	signals:
		void playGsmSample(char *data, int length);

	private:
		QSemaphore wsem;
		QValueList<struct gsm_sample> queue;
		QMutex mutex;

	friend class VoiceManager;
};

class RecordThread : public QThread, public QObject
{
	Q_OBJECT

	public:
		RecordThread();
		void run();

	signals:
		void recordSample(char *data, int length);

	private:
		QSemaphore wsem;
		QValueList<struct gsm_sample> queue;
		QMutex mutex;

	friend class VoiceManager;
};

#endif
