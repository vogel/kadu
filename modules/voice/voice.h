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

#include "../dcc/dcc.h"

class PlayThread;
class RecordThread;

struct gsm_sample {
	char *data;
	int length;
};

class VoiceChatDialog : public QDialog
{
	Q_OBJECT

	private:
		static QMap<DccSocket*, VoiceChatDialog*> Dialogs;
		DccSocket* Socket;

	public:
		VoiceChatDialog(DccSocket* socket);
		~VoiceChatDialog();
		static VoiceChatDialog* bySocket(DccSocket* socket);
		static void destroyAll();
		static void sendDataToAll(char* data, int length);
};

class VoiceManager : public QObject
{
	Q_OBJECT

	private:
		friend class PlayThread;
		friend class RecordThread;
		PlayThread* pt;
		RecordThread* rt;
		gsm voice_enc;
		gsm voice_dec;
		/**
			Przechowuje informacje o wys³anych
			request'ach CTCP. Je¶li kto¶ jest
			za nat'em i chcemy rozpocz±æ z nim
			rozmowê g³osow± to wysy³amy pro¶bê
			o po³±czenie a jego numer uin jest
			zapamiêtywany.
		**/
		QMap<UinType, bool> Requests;

		void resetCoder();
		void resetDecoder();
		void askAcceptVoiceChat(DccSocket* socket);

	private slots:
		void playGsmSampleReceived(char *data, int length);
		void recordSampleReceived(char *data, int length);
		void mainDialogKeyPressed(QKeyEvent* e);
		void userBoxMenuPopup();
		void makeVoiceChat();
		void connectionBroken(DccSocket* socket);
		void callbackReceived(DccSocket* socket);
		void dccError(DccSocket* socket);
		void dccEvent(DccSocket* socket);
		void socketDestroying(DccSocket* socket);

	public:
		VoiceManager(QObject *parent=0, const char *name=0);
		~VoiceManager();
		void setup();
		void free();
		void resetCodec();
		void addGsmSample(char *data, int length);

	signals:
		void setupSoundDevice();
		void freeSoundDevice();
		void playSample(char *data, int length);
		void recordSample(char *data, int length);
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

extern VoiceManager *voice_manager;

#endif
