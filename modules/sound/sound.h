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
		void soundPlayer(bool value, bool toolbarChanged=false);
		void onCreateConfigDialog();
		void onApplyConfigDialog();
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
		void notifySound(const UinType uin, const unsigned int oldstatus, const unsigned int status);

	public slots:
		void play(const QString &path, bool force=false);
		void setMute(const bool& enable);

	public:
		SoundManager(const QString& name, const QString& configname);
		bool isMuted();
		int timeAfterLastSound();		

	signals:
		void playSound(const QString &sound, bool volCntrl, double vol);
		void playOnMessage(UinsList senders, const QString &sound, const QString &msg, bool volCntrl, double vol);
		void playOnChat(UinsList senders, const QString &sound, const QString &msg, bool volCntrl, double vol);
		void playOnNotify(const UinType uin, const QString &sound, bool volCntrl, double vol);
};

extern SoundManager* sound_manager;

#endif
