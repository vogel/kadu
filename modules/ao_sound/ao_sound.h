#ifndef KADU_AO_SOUND_H
#define KADU_AO_SOUND_H

#include <QtCore/QObject>
#include <QtCore/QString>
#include <QtCore/QThread>
#include <QtCore/QMutex>
#include <QtCore/QSemaphore>
#include <QtCore/QList>

class SndParams2
{
	public:
		SndParams2(QString fm, bool volCntrl=false, float vol=1) :
			filename(fm), volumeControl(volCntrl), volume(vol) {}
		SndParams2(const SndParams2 &p) : 
			filename(p.filename), volumeControl(p.volumeControl), volume(p.volume) {}
		SndParams2() {}

		QString filename;
		bool volumeControl;
		float volume;
};

class AOPlayThread : public QThread
{
	public:
		QMutex mutex;
		QSemaphore *semaphore; //s³u¿y do powiadamiania o d¼wiêku w kolejce
		bool end;
		QList<SndParams2> list;
		
		AOPlayThread();
		~AOPlayThread();
		void run();
	private:
		static bool play(const char *path, bool &checkAgain, bool volCntrl=false, float vol=1);
};

class AOPlayerSlots : public QObject
{
	Q_OBJECT
	private:
		AOPlayThread *thread;
	public:
		AOPlayerSlots(QObject *parent=0, const char *name=0);
		~AOPlayerSlots();
		bool isOk();
	private slots:
		void playSound(const QString &s, bool volCntrl, double vol);
};

extern AOPlayerSlots *ao_player_slots;

#endif

