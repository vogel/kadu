#ifndef SOUND_H
#define SOUND_H

#include <qobject.h>
#include <qthread.h>
#include <qsemaphore.h>

extern bool mute;

void playSound(const QString &sound, const QString player = QString::null);

class SoundSlots: public QObject
{
	Q_OBJECT
	
	public:
		static void initModule();
	public slots:
		void soundPlayer(bool value);
		void onCreateConfigDialog();
		void choosePlayerFile();
		void chooseMsgFile();
		void chooseMsgTest();
		void chooseChatFile();
		void chooseChatTest();	

};

#endif
