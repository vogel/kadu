#ifndef KADU_SOUND_SLOTS
#define KADU_SOUND_SLOTS

#include <qmap.h>
#include <qstringlist.h>

#include "message_box.h"
#include "sound.h"

class SelectFile;

class Action;

/** @ingroup sound
 * @{
 */
class SoundConfigurationWidget : public NotifierConfigurationWidget
{
	Q_OBJECT

	QMap<QString, QString> soundFiles;
	QString currentNotifyEvent;

	SelectFile *soundFileSelectFile;

private slots:
	void test();

public:
	SoundConfigurationWidget(QWidget *parent = 0, char *name = 0);
	virtual ~SoundConfigurationWidget();

	virtual void loadNotifyConfigurations() {};
	virtual void saveNotifyConfigurations();
	virtual void switchToEvent(const QString &event);

public slots:
	void themeChanged(int index);

};

class SoundSlots : public QObject, public ConfigurationAwareObject
{
	Q_OBJECT
	private:
		Action* mute_action;
		QMap<QString, QString> soundfiles;
		QStringList soundNames;
		QStringList soundTexts;

		MessageBox* SamplePlayingTestMsgBox;
		SoundDevice SamplePlayingTestDevice;
		int16_t*    SamplePlayingTestSample;

		MessageBox* SampleRecordingTestMsgBox;
		SoundDevice SampleRecordingTestDevice;
		int16_t*    SampleRecordingTestSample;

		MessageBox* FullDuplexTestMsgBox;
		SoundDevice FullDuplexTestDevice;
		int16_t*    FullDuplexTestSample;

	private slots:
		void muteActionActivated(const UserGroup* users, const QWidget* source, bool is_on);
		void setMuteActionState();
		void muteUnmuteSounds();
		void testSamplePlaying();
		void samplePlayingTestSamplePlayed(SoundDevice device);
		void testSampleRecording();
		void sampleRecordingTestSampleRecorded(SoundDevice device);
		void sampleRecordingTestSamplePlayed(SoundDevice device);
		void testFullDuplex();
		void fullDuplexTestSampleRecorded(SoundDevice device);
		void closeFullDuplexTest();
	protected:
		void configurationUpdated();
	public:
		SoundSlots(QObject *parent=0, const char *name=0);
		~SoundSlots();
};

/** @} */
#endif
