#ifndef KADU_DSP_SOUND_H
#define KADU_DSP_SOUND_H
#include <qobject.h>
#include <qstring.h>
#include <qthread.h>
#include <qstringlist.h>
#include <qmutex.h>
#include <qsemaphore.h>
#include "misc.h"

class OSSPlayThread : public QThread
{
	public:
		QMutex mutex;
		QSemaphore *semaphore; //s³u¿y do powiadamiania o d¼wiêku w kolejce
		bool end;
		QStringList list;
		
		OSSPlayThread();
		~OSSPlayThread();
		void run();
};

class OSSPlayerSlots : public QObject
{
	Q_OBJECT
	public:
		OSSPlayerSlots(QObject *parent=0, const char *name=0);
		~OSSPlayerSlots();
		bool error;
	private slots:
		void play(const QString &s, bool volCntrl, double vol, const QString &device=QString::null);
		
		void playSound(const QString &s, bool volCntrl, double vol);
	private:
		OSSPlayThread *thread;
};

extern OSSPlayerSlots *oss_player_slots;

#endif

