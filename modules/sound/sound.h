#ifndef SOUND_H
#define SOUND_H

#include <qobject.h>
#include <qstring.h>
#include <qdatetime.h>
#include <qstringlist.h>
#include "config_file.h"

#include "events.h"
#include "misc.h"

class SoundSlots: public QObject
{
	Q_OBJECT
	private:
		int muteitem;

	private slots:
		void soundPlayer(bool value);
		void onCreateConfigDialog();
		void onDestroyConfigDialog();
		void chooseSoundTheme(const QString& string);
		void choosePlayerFile();
		void chooseSoundFile();
		void clearSoundFile();
		void testSoundFile();
		void selectedPaths(const QStringList& paths);
		void muteUnmuteSounds();
		void playingSound(const QString& file);
	public:
		SoundSlots();

		
};

extern SoundSlots* soundslots;

class SoundManager : public Themes
{
    Q_OBJECT
	private:
		QTime lastsoundtime;
		bool mute;

	private slots:
		void chatSound(UinsList senders,const QString& msg,time_t time, bool& grab);
		void messageSound(UinsList senders,const QString& msg,time_t time);
		void notifySound(const uin_t uin, const unsigned int oldstatus, const unsigned int status);


	public:
		SoundManager(const QString& name, const QString& configname);
		bool getMute();
		void setMute(const bool& enable);
		void playSound(const QString &sound, const QString player = QString::null);
		int timeAfterLastSound();		
		static void initModule();

	signals:
		void playFile(const QString& file);

		void playOnMessage(UinsList senders);
		void playOnChat(UinsList senders);
		void playOnNotify(const uin_t uin);


};

extern SoundManager* sound_manager;

#endif
