#ifndef SOUND_H
#define SOUND_H

#include <qobject.h>
#include <qthread.h>
#include <qsemaphore.h>
//#include <kde/artsc/artsc.h>

extern bool mute;

void playSound(const QString &sound, const QString player = QString::null);

class SoundDevice;

class PlayThread : public QThread
{
	public:
		PlayThread(SoundDevice *snddev);
		~PlayThread();
		void run();

	private:
		SoundDevice *snddev;
		QSemaphore *semwait;
		QSemaphore *semplay;

	friend class SoundDevice;
};

class RecordThread : public QThread
{
	public:
		RecordThread(SoundDevice *snddev);
		~RecordThread();
		void run();

	private:
		SoundDevice *snddev;
		QSemaphore *semwait;
		QSemaphore *semrec;

	friend class SoundDevice;
};

class SoundDevice : public QObject
{
	Q_OBJECT
	public:
		SoundDevice(const int freq, const int bits, const int chans, QObject *parent = 0, const char *name = 0);
		virtual ~SoundDevice();
		void play(char *buf, int size);
		bool playFinished();
		void record(char *buf, int size);
		bool recordFinished();

	private:
		bool playfinished;
		bool recfinished;

	protected:
		virtual void doPlaying() = 0;
		virtual void doRecording() = 0;

		RecordThread *rt;
		PlayThread *pt;
		int freq;
		int bits;
		int chans;
		char *playbuf;
		int playbufsize;
		char *recbuf;
		int recbufsize;

	friend class PlayThread;
	friend class RecordThread;
};


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

/*class ArtsSoundDevice : public SoundDevice
{
	Q_OBJECT
	public:
		ArtsSoundDevice(const int freq, const int bits, const int chans, QObject *parent = 0, const char *name = 0);
		~ArtsSoundDevice();

	private:
		arts_stream_t playstream;
		arts_stream_t recstream;

	protected:
		void doPlaying();
		void doRecording();

	friend class PlayThread;
	friend class RecordThread;
};*/

#endif
