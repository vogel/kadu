#ifndef DCC_H
#define DCC_H

#include <qdialog.h>
#include <qvbox.h>
#include <qprogressbar.h>
#include <qlabel.h>
#include <qsocketnotifier.h>

#include "libgadu.h"
#include "../config.h"

enum DccFileDialogType {
	DCC_TYPE_SEND,
	DCC_TYPE_GET
};

enum DccType {
	DCC_TYPE_FILE,
	DCC_TYPE_VOICE
};

class dccSocketClass;

class DccFileDialog : public QDialog {
	Q_OBJECT
	public:
		DccFileDialog(dccSocketClass *dccsocket, int type = DCC_TYPE_GET,
			QDialog *parent = 0, const char *name = 0);
		~DccFileDialog();
		void printFileInfo(struct gg_dcc *dccsock);
		void updateFileInfo(struct gg_dcc *dccsock);

		bool dccFinished;				
	protected:
		void closeEvent(QCloseEvent *e);

		dccSocketClass *dccsocket;
		QLabel *l_offset;
		QProgressBar *p_progress;
		QVBox *vbox1;
		long long int prevPercent;
		QTime *time;
		int prevOffset;
		int type;
};

class DccVoiceDialog : public QDialog {
	Q_OBJECT

	public:
		DccVoiceDialog(QDialog *parent = 0, const char *name = 0);

	protected:
		void closeEvent(QCloseEvent *e);

	signals:
		void cancelVoiceChat();
};

enum dccSocketState {
	DCC_SOCKET_TRANSFERRING,
	DCC_SOCKET_CONNECTION_BROKEN,
	DCC_SOCKET_TRANSFER_ERROR,
	DCC_SOCKET_TRANSFER_FINISHED,
	DCC_SOCKET_COULDNT_OPEN_FILE,
	DCC_SOCKET_TRANSFER_DISCARDED,
	DCC_SOCKET_VOICECHAT_DISCARDED
};

class dccSocketClass : public QObject {
	Q_OBJECT
	public:
		dccSocketClass(struct gg_dcc *dcc_sock, int type = DCC_TYPE_FILE);
		~dccSocketClass();
		void initializeNotifiers();
		void watchDcc(int check);

		int type;
		int state;
		static int count;

	public slots:
		void setState(int pstate);
		void voiceDataRecorded(char *data, int length);
		void cancelVoiceChatReceived();

	signals:
		void dccFinished(dccSocketClass *dcc);

	protected:
		void askAccept();
#ifdef VOICE_ENABLED
		void askAcceptVoiceChat();
#endif
		QString selectFile();

		QSocketNotifier *snr;
		QSocketNotifier *snw;
		struct gg_dcc *dccsock;
		struct gg_event *dccevent;
		DccFileDialog *filedialog;
		DccVoiceDialog *voicedialog;
		bool in_watchDcc;

	protected slots:
		void dccDataReceived();
		void dccDataSent();
};

#endif
