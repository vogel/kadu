#ifndef SOUND_H
#define SOUND_H

#include <qobject.h>
#include <qstring.h>
#include <qdatetime.h>
#include <qstringlist.h>
#include "config_file.h"

#include "events.h"
#include "misc.h"
#include "modules.h"

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
		void chooseSoundFile();
		void clearSoundFile();
		void testSoundFile();
		void selectedPaths(const QStringList& paths);
		void muteUnmuteSounds();
	public:
		SoundSlots();
		~SoundSlots();
};

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
		int timeAfterLastSound();		
		void playTestSound(const QString &);

	signals:
		void playOnTestSound(const QString &sound, bool volCntrl, double vol);
		void playOnMessage(UinsList senders, const QString &sound, const QString &msg, bool volCntrl, double vol);
		void playOnChat(UinsList senders, const QString &sound, const QString &msg, bool volCntrl, double vol);
		void playOnNotify(const uin_t uin, const QString &sound, bool volCntrl, double vol);
};

inline SoundManager* soundManager()
{
	SoundManager** snd=(SoundManager**)modules_manager->moduleSymbol("sound","sound_manager");
	if(snd==NULL)
		return NULL;
	return *snd;
}

#endif
