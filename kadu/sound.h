#ifndef SOUND_H
#define SOUND_H

#include <qobject.h>
#include <qdatetime.h>
#include <qstringlist.h>
#include "config_file.h"

class SoundSlots: public QObject
{
	Q_OBJECT
	
	public slots:
		void soundPlayer(bool value);
		void onCreateConfigDialog();
		void onDestroyConfigDialog();
		void chooseSoundTheme(const QString& string);
		void choosePlayerFile();
		void chooseSoundFile();
		void clearSoundFile();
		void testSoundFile();
		void selectedPaths(const QStringList& paths);

};

class SoundManager
{
	private:
		QStringList ThemesList;
		QStringList ThemesPaths;
		QValueList<ConfigFileEntry> entries;
		QStringList getSubDirs(const QString& path);
		QString fixFileName(const QString& path,const QString& fn);
		QTime lastsoundtime;
	public:
		SoundManager();
		static void initModule();
		bool mute;
		void playSound(const QString &sound, const QString player = QString::null);
		const QStringList& themes();
		void setSoundTheme(const QString& theme);
		void setSoundPaths(const QStringList& paths);
		QString themePath(const QString& theme="");
		QString getThemeEntry(const QString& name);
		int timeAfterLastSound();
};

extern SoundManager soundmanager;
#endif
