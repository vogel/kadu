#ifndef KADU_DSP_SOUND_H
#define KADU_DSP_SOUND_H
#include <qobject.h>
#include <qstring.h>
#include <qthread.h>
#include <qstringlist.h>
#include <qmutex.h>
#include <qsemaphore.h>
#include "misc.h"

class DirectPlayThread : public QThread
{
	public:
		QMutex mutex;
		QSemaphore *semaphore; //s³u¿y do powiadamiania o d¼wiêku w kolejce
		bool end;
		QStringList list;
		
		DirectPlayThread();
		~DirectPlayThread();
		void run();
};

class DirectPlayerSlots : public QObject
{
	Q_OBJECT
	public:
		DirectPlayerSlots();
		~DirectPlayerSlots();
		bool error;
	private slots:
		void play(const QString &s, bool volCntrl, double vol, const QString &device=QString::null);
		
		void playSound(const QString &s, bool volCntrl, double vol);

		void playNewMessage(UinsList senders, const QString &sound, bool volCntrl, double vol, const QString &msg);
		void playNewChat(UinsList senders, const QString &sound, bool volCntrl, double vol, const QString &msg);
		void playConnectionError(const QString &sound, bool volCntrl, double vol, const QString &msg);
		void playNotify(const UinType uin, const QString &sound, bool volCntrl, double vol);
		
		void playMessage(const QString &sound, bool volCntrl, double vol, const QString &from, const QString &type, const QString &msg, const UserListElement *ule);

	private:
		DirectPlayThread *thread;
};

extern DirectPlayerSlots *directPlayerObj;

#endif

