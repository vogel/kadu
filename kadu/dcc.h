#ifndef DCC_H
#define DCC_H

#include <qdialog.h>
#include <qvbox.h>
#include <qprogressbar.h>
#include <qlabel.h>
#include <qsocketnotifier.h>
#include <qprocess.h>
#include "../libgadu/lib/libgadu.h"

enum DccGetType {
	DCC_TYPE_SEND,
	DCC_TYPE_GET
};

class dccSocketClass;

class DccGet : public QDialog {
	Q_OBJECT
	public:
		DccGet(dccSocketClass *dccsocket, int type=DCC_TYPE_GET, QDialog* parent=0, const char *name=0);
		~DccGet();
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

enum dccSocketState {
	DCC_SOCKET_TRANSFERRING,
	DCC_SOCKET_CONNECTION_BROKEN,
	DCC_SOCKET_TRANSFER_ERROR,
	DCC_SOCKET_TRANSFER_FINISHED,
	DCC_SOCKET_COULDNT_OPEN_FILE,
	DCC_SOCKET_TRANSFER_DISCARDED
};

class dccSocketClass : public QObject {
	Q_OBJECT
	public:
		dccSocketClass(struct gg_dcc *dcc_sock);
		~dccSocketClass();
		void initializeNotifiers();
		void watchDcc(int check);

		int state;

	public slots:
		void setState(int pstate);

	signals:
		void dccFinished(dccSocketClass *dcc);

	protected:
		void askAccept();
		QString selectFile();

		QSocketNotifier *snr;
		QSocketNotifier *snw;
		struct gg_dcc *dccsock;
		struct gg_event *dccevent;
		DccGet *dialog;
		QProcess *playprocess;
		QProcess *recordprocess;

	protected slots:
		void dccDataReceived();
		void dccDataSent();
};

#endif
