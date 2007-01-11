#ifndef KADU_VOICE_H
#define KADU_VOICE_H

#include <qdialog.h>
#include <qthread.h>
#include <qmutex.h>
#include <qsemaphore.h>
#include <qvaluelist.h>
extern "C" {
#include "libgsm/inc/gsm.h"
};

#include "../dcc/dcc.h"
#include "../sound/sound.h"

class PlayThread;
class RecordThread;

/**
 * @defgroup voice Voice
 * @{
 */
struct gsm_sample
{
	char *data;
	int length;
};

class VoiceChatDialog : public QDialog
{
	Q_OBJECT

	private:
		static QMap<DccSocket *, VoiceChatDialog *> Dialogs;
		DccSocket* Socket;

	public:
		VoiceChatDialog(DccSocket *socket);
		~VoiceChatDialog();
		static VoiceChatDialog *bySocket(DccSocket *socket);
		static void destroyAll();
		static void sendDataToAll(char *data, int length);

		bool chatFinished;
};

class VoiceManager : public QObject
{
	Q_OBJECT

	private:
		MessageBox *GsmEncodingTestMsgBox;
		SoundDevice GsmEncodingTestDevice;
		gsm GsmEncodingTestHandle;
		int16_t *GsmEncodingTestSample;
		gsm_frame *GsmEncodingTestFrames;
		int GsmEncodingTestCurrFrame;
		SoundDevice device;

		PlayThread *playThread;
		RecordThread *recordThread;
		gsm voice_enc;
		gsm voice_dec;
		QValueList<UinType> direct;

		void resetCoder();
		void resetDecoder();
		void askAcceptVoiceChat(DccSocket *socket);

	private slots:
		void testGsmEncoding();
		void gsmEncodingTestSampleRecorded(SoundDevice device);
		void gsmEncodingTestSamplePlayed(SoundDevice device);
		void playGsmSampleReceived(char *data, int length);
		void recordSampleReceived(char *data, int length);
		void mainDialogKeyPressed(QKeyEvent *e);
		void userBoxMenuPopup();
		void makeVoiceChat();
		void connectionBroken(DccSocket *socket);
		void dccError(DccSocket *socket);
		void dccEvent(DccSocket *socket, bool &lock);
		void socketDestroying(DccSocket *socket);
		void setState(DccSocket *socket);

	public:
		VoiceManager(QObject *parent = 0, const char *name = 0);
		~VoiceManager();
		int setup();
		void free();
		void resetCodec();
		void addGsmSample(char *data, int length);

	public slots:
		void makeVoiceChat(UinType dest);
};

class PlayThread : public QObject, public QThread
{
	Q_OBJECT

	public:
		PlayThread();
		void run();
		void endThread();
		void addGsmSample(char *data, int length);

	signals:
		void playGsmSample(char *data, int length);

	private:
		QSemaphore wsem;
		void waitForData(); //czeka na nowe dane
		void moreData(); //daje znaæ, ¿e s± nowe dane

		QValueList<struct gsm_sample> samples;
		QMutex samplesMutex; // chroni dostêp do samples

		bool end;
//		QMutex endMutex; //chroni dostêp do end
};

class RecordThread : public QObject, public QThread
{
	Q_OBJECT

	public:
		RecordThread();
		void run();
		void endThread();

	signals:
		void recordSample(char *data, int length);

	private:
		bool end;
//		QMutex endMutex; //chroni dostêp do end
};

extern VoiceManager *voice_manager;

/** @} */

#endif
