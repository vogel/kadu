#ifndef KADU_SOUND_SLOTS
#define KADU_SOUND_SLOTS

#include <QtCore/QMap>
#include <QtCore/QStringList>

#include "gui/windows/message-box.h"

#include "gui/widgets/configuration/notifier-configuration-widget.h"

#include "sound.h"

class SelectFile;

class ActionDescription;

/** @ingroup sound
 * @{
 */
class SoundConfigurationWidget : public NotifierConfigurationWidget
{
	Q_OBJECT

	QMap<QString, QString> soundFiles;
	QString currentNotifyEvent;

	SelectFile *soundFileSelectFile;
	QLabel *warning;

private slots:
	void test();

public:
	SoundConfigurationWidget(QWidget *parent = 0);
	virtual ~SoundConfigurationWidget();

	virtual void loadNotifyConfigurations() {}
	virtual void saveNotifyConfigurations();
	virtual void switchToEvent(const QString &event);

public slots:
	void themeChanged(int index);

signals:
	void soundFileEdited();

};

class SoundSlots : public QObject, public ConfigurationAwareObject
{
	Q_OBJECT

	ActionDescription* mute_action;
	QMap<QString, QString> soundfiles;
	QStringList soundNames;
	QStringList soundTexts;

	MessageBox* SamplePlayingTestMsgBox;
	SoundDevice SamplePlayingTestDevice;
	qint16*    SamplePlayingTestSample;

	MessageBox* SampleRecordingTestMsgBox;
	SoundDevice SampleRecordingTestDevice;
	qint16*    SampleRecordingTestSample;

	MessageBox* FullDuplexTestMsgBox;
	SoundDevice FullDuplexTestDevice;
	qint16*    FullDuplexTestSample;

private slots:
	void muteActionActivated(QAction *action, bool is_on);
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
	SoundSlots(bool firstLoad, QObject *parent = 0);
	~SoundSlots();
public slots:
	void themeChanged(const QString &theme);
};

/** @} */
#endif
