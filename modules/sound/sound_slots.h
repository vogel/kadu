#ifndef KADU_SOUND_SLOTS
#define KADU_SOUND_SLOTS
#include <qobject.h>
#include <qmap.h>
#include <qstringlist.h>
#include "message_box.h"
#include "sound.h"

class SoundSlots : public QObject
{
	Q_OBJECT
	private:
		int muteitem;
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
		void soundPlayer(bool value, bool toolbarChanged=false);
		void onCreateTabSounds();
		void onApplyTabSounds();
		void chooseSoundTheme(const QString& string);
		void chooseSoundFile();
		void clearSoundFile();
		void testSoundFile();
		void selectedPaths(const QStringList& paths);
		void muteUnmuteSounds();
		void testSamplePlaying();
		void samplePlayingTestSamplePlayed(SoundDevice device);
		void testSampleRecording();
		void sampleRecordingTestSampleRecorded(SoundDevice device);
		void sampleRecordingTestSamplePlayed(SoundDevice device);
		void testFullDuplex();
		void fullDuplexTestSampleRecorded(SoundDevice device);
		void closeFullDuplexTest();

	public:
		SoundSlots(QObject *parent=0, const char *name=0);
		~SoundSlots();
};

#endif
