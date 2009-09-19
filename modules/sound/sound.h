#ifndef KADU_SOUND_H
#define KADU_SOUND_H

#include <QtCore/QObject>
#include <QtCore/QEvent>
#include <QtCore/QString>
#include <QtCore/QDateTime>
#include <QtCore/QStringList>
#include <QtCore/QMap>
#include <QtCore/QThread>
#include <QtCore/QMutex>
#include <QtCore/QSemaphore>

#include "gui/windows/main-configuration-window.h"
#include "misc/misc.h"
#include "notify/notifier.h"
#include "modules.h"
#include "themes.h"

#include "sound_exports.h"

class Notification;
class PathListEdit;

/**
 * @defgroup sound Sound
 * The sound module.
 * @{
 */

typedef void* SoundDevice;

/**
**/
enum SoundDeviceType {RECORD_ONLY, PLAY_ONLY, PLAY_AND_RECORD};

class SoundEvent : public QEvent
{
	SoundDevice Data;
	static int EventNumber;

public:
	SoundEvent(SoundDevice data) : QEvent((QEvent::Type)EventNumber) { Data = data; }
	SoundDevice data() { return Data; }

	static int eventNumber() { return EventNumber; }
};

class SamplePlayThread : public QThread
{
	Q_OBJECT

	private:
		SoundDevice Device;
		const qint16* Sample;
		int SampleLen;
		bool Stopped;
		QSemaphore PlayingSemaphore;
		QSemaphore SampleSemaphore;

	protected:
		virtual void run();
		virtual bool event(QEvent* event);

	public:
		SamplePlayThread(SoundDevice device);
		void playSample(const qint16* data, int length);
		void stop();

	signals:
		void samplePlayed(SoundDevice device);
};

class SampleRecordThread : public QThread
{
	Q_OBJECT

	private:
		SoundDevice Device;
		qint16* Sample;
		int SampleLen;
		bool Stopped;
		QSemaphore RecordingSemaphore;
		QSemaphore SampleSemaphore;

	protected:
		virtual void run();
		virtual bool event(QEvent* event);

	public:
		SampleRecordThread(SoundDevice device);
		void recordSample(qint16* data, int length);
		void stop();

	signals:
		void sampleRecorded(SoundDevice device);
};

class SndParams
{
	public:
		SndParams(QString fm = QString::null, bool volCntrl = false, float vol = 1);
		SndParams(const SndParams &p);

		QString filename;
		bool volumeControl;
		float volume;
};

class SoundPlayThread : public QThread
{
	public:
		SoundPlayThread();
		~SoundPlayThread();
		void run();
		void tryPlay(const char *path, bool volCntrl=false, float volume=1.0);
		void endThread();

	private:
		static bool play(const char *path, bool volCntrl=false, float volume=1.0);
		QMutex mutex;
		QSemaphore *semaphore;
		bool end;
		QList<SndParams> list;
};

class SoundConfigurationWidget;

class SOUNDAPI SoundManager : public Notifier, public ConfigurationUiHandler
{
    Q_OBJECT

		Themes *themes;
		ConfigComboBox *themesComboBox;
		PathListEdit *themesPaths;
		SoundConfigurationWidget *configurationWidget;

		friend class SamplePlayThread;
		friend class SampleRecordThread;
		QTime lastsoundtime;
		bool mute;
		QMap<SoundDevice, SamplePlayThread*> PlayingThreads;
		QMap<SoundDevice, SampleRecordThread*> RecordingThreads;
		SoundPlayThread *play_thread;

		int simple_player_count;
		virtual void connectNotify(const char *signal);
		virtual void disconnectNotify(const char *signal);

		void playSound(const QString &soundName);

		void copyConfiguration(const QString &fromEvent, const QString &toEvent) {}

		void applyTheme(const QString &themeName);

		void import_0_6_5_configuration();
		void createDefaultConfiguration();

	private slots:
		void setSoundThemes();
		void configurationWindowApplied();
		void soundFileEdited();

	public slots:
		void play(const QString &path, bool force = false);
		void play(const QString &path, bool volCntrl, double vol);
		void setMute(const bool& enable);
		void stop();

	public:

		SoundManager(bool firstLoad, const QString& name, const QString& configname);
		~SoundManager();

		virtual void mainConfigurationWindowCreated(MainConfigurationWindow *mainConfigurationWindow);
		virtual NotifierConfigurationWidget *createConfigurationWidget(QWidget *parent = 0);

		virtual void notify(Notification *notification);

		Themes *theme();

		bool isMuted() const;
		int timeAfterLastSound() const;

		SoundDevice openDevice(SoundDeviceType type, int sample_rate, int channels = 1);
		void closeDevice(SoundDevice device);
		void enableThreading(SoundDevice device);
		void setFlushingEnabled(SoundDevice device, bool enabled);
		bool playSample(SoundDevice device, const qint16* data, int length);
		bool recordSample(SoundDevice device, qint16* data, int length);

	signals:
		void playSound(const QString &sound, bool volCntrl, double vol);
		void samplePlayed(SoundDevice device);
		void sampleRecorded(SoundDevice device);
		void openDeviceImpl(SoundDeviceType type, int sample_rate, int channels, SoundDevice* device);
		void closeDeviceImpl(SoundDevice device);
		void playSampleImpl(SoundDevice device, const qint16* data, int length, bool *result);
		void recordSampleImpl(SoundDevice device, qint16* data, int length, bool *result);
		/**
		**/
		void setFlushingEnabledImpl(SoundDevice device, bool enabled);
};

extern SOUNDAPI SoundManager* sound_manager;
/** @} */

#endif
