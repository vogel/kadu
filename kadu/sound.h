#ifndef SOUND_H
#define SOUND_H

#include <qobject.h>
#include <qthread.h>

class SoundDevice;

class PlayThread : public QThread
{
	public:
		PlayThread(SoundDevice *snddev);
		void run();

	private:
		SoundDevice *snddev;
};

class RecordThread : public QThread
{
	public:
		RecordThread(SoundDevice *snddev);
		void run();

	private:
		SoundDevice *snddev;
};

class SoundDevice : public QObject
{
	Q_OBJECT
	public:
		SoundDevice(int freq, int bits, int chans, QObject *parent = 0, const char *name = 0);

	private:
	
	protected:
		RecordThread *rt;
		PlayThread *pt;
		int freq;
		int bits;
		int chans;

	friend class PlayThread;
	friend class RecordThread;
};

#endif
