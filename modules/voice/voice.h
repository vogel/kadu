#ifndef VOICE_H
#define VOICE_H

#include <qdialog.h>
#include <qthread.h>
#include <qmutex.h>
#include <qsemaphore.h>
#include <qvaluelist.h>
extern "C" {
#include "libgsm/inc/gsm.h"
};

#include "dcc.h"

class PlayThread;
class RecordThread;

struct gsm_sample {
	char *data;
	int length;
};

class VoiceManager : public QObject {
	Q_OBJECT

	public:
		VoiceManager();
		~VoiceManager();
		void setup();
		void free();
		void resetCodec();
		void addGsmSample(char *data, int length);

	signals:
		void setupSoundDevice();
		void freeSoundDevice();
		void gsmSampleRecorded(char *data, int length);
		void playSample(char *data, int length);
		void recordSample(char *data, int length);

	private slots:
		void playGsmSampleReceived(char *data, int length);
		void recordSampleReceived(char *data, int length);
		void mainDialogKeyPressed(QKeyEvent* e);
		void userBoxMenuPopup();
		void makeVoiceChat();
		void dccFinished(DccSocket* dcc);

	private:
		void resetCoder();
		void resetDecoder();

		PlayThread *pt;
		gsm voice_enc;
		RecordThread *rt;
		gsm voice_dec;

	friend class PlayThread;
	friend class RecordThread;
};

class PlayThread : public QObject, public QThread
{
	Q_OBJECT

	public:
		PlayThread();
		void run();

	signals:
		void playGsmSample(char *data, int length);

	private:
		QSemaphore wsem, rsem;
		QValueList<struct gsm_sample> queue;
		QMutex mutex;

	friend class VoiceManager;
};

class RecordThread : public QObject, public QThread
{
	Q_OBJECT

	public:
		RecordThread();
		void run();

	signals:
		void recordSample(char *data, int length);

	private:
		QSemaphore rsem;
		QValueList<struct gsm_sample> queue;
		QMutex mutex;

	friend class VoiceManager;
};

class DccVoiceDialog : public QDialog
{
	Q_OBJECT

	public:
		DccVoiceDialog(QDialog *parent = 0, const char *name = 0);

	protected:
		void closeEvent(QCloseEvent *e);

	signals:
		void cancelVoiceChat();
};

class VoiceSocket : public DccSocket
{
	Q_OBJECT
	
	private:
		DccVoiceDialog *voicedialog;
		void askAcceptVoiceChat();
		
	private slots:
		void cancelVoiceChatReceived();
		void voiceDataRecorded(char *data, int length);

	protected:
		virtual void connectionBroken();
		virtual void dccError();
		virtual void dccEvent();

	public:
		VoiceSocket(struct gg_dcc *dcc_sock);
		~VoiceSocket();
		virtual void initializeNotifiers();
};

extern VoiceManager *voice_manager;

#endif
