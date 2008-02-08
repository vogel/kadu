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
#include "../dcc/dcc_handler.h"
#include "../sound/sound.h"

#include "main_configuration_window.h"

class ChatWidget;
class PlayThread;
class RecordThread;

class QCheckBox;

/**
 * @defgroup voice Voice
 * @{
 */
struct gsm_sample
{
	char *data;
	int length;
};

class VoiceChatDialog : public QDialog, public DccHandler
{
	Q_OBJECT

	static QValueList<VoiceChatDialog *> VoiceChats;
	DccSocket* Socket;

public:
	VoiceChatDialog();
	~VoiceChatDialog();

	bool addSocket(DccSocket *socket);
	void removeSocket(DccSocket *socket);

	int dccType() { return GG_SESSION_DCC_VOICE; }

	bool socketEvent(DccSocket *socket, bool &lock);

	void connectionDone(DccSocket *socket) {}
	void connectionError(DccSocket *socket) {}

	void connectionAccepted(DccSocket *socket) {}
	void connectionRejected(DccSocket *socket) {}

	static void destroyAll();
	static void sendDataToAll(char *data, int length);

	void sendData(char *data, int length);

	bool chatFinished;
};

class VoiceManager : public ConfigurationUiHandler, public DccHandler
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

	void resetCoder();
	void resetDecoder();
	bool askAcceptVoiceChat(DccSocket *socket);

	QCheckBox *testFast;
	QCheckBox *testCut;

	void createDefaultConfiguration();

	void makeVoiceChat(UinType dest);

private slots:
	void makeVoiceChat();

	void testGsmEncoding();
	void gsmEncodingTestSampleRecorded(SoundDevice device);
	void gsmEncodingTestSamplePlayed(SoundDevice device);
	void playGsmSampleReceived(char *data, int length);
	void recordSampleReceived(char *data, int length);
	void mainDialogKeyPressed(QKeyEvent *e);
	void chatKeyPressed(QKeyEvent *e, ChatWidget *chatWidget, bool &handled);
	void userBoxMenuPopup();

	void chatCreated(ChatWidget *chat);
	void chatDestroying(ChatWidget *chat);

public:
	VoiceManager();
	virtual ~VoiceManager();

	int setup();
	void free();
	void resetCodec();
	void addGsmSample(char *data, int length);

	bool addSocket(DccSocket *socket);
	void removeSocket(DccSocket *socket) {}

	int dccType() { return GG_SESSION_DCC_VOICE; }

	bool socketEvent(DccSocket *socket, bool &lock);

	void connectionDone(DccSocket *socket) {}
	void connectionError(DccSocket *socket) {}

	void connectionAccepted(DccSocket *socket) {}
	void connectionRejected(DccSocket *socket) {}

	virtual void mainConfigurationWindowCreated(MainConfigurationWindow *mainConfigurationWindow);

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
//	QMutex endMutex; //chroni dostêp do end
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
//	QMutex endMutex; //chroni dostêp do end
};

extern VoiceManager *voice_manager;

/** @} */

#endif
