#ifndef DCC_H
#define DCC_H

#include <qdialog.h>
#include <qlayout.h> 
#include <qprogressbar.h>
#include <qlabel.h>
#include <qsocketnotifier.h>
#include <qhostaddress.h>
#include <qevent.h>

#include "kadu-config.h"
#include "gadu.h"


enum dccSocketState {
	DCC_SOCKET_TRANSFERRING,
	DCC_SOCKET_CONNECTION_BROKEN,
	DCC_SOCKET_TRANSFER_ERROR,
	DCC_SOCKET_TRANSFER_FINISHED,
	DCC_SOCKET_COULDNT_OPEN_FILE,
	DCC_SOCKET_TRANSFER_DISCARDED,
	DCC_SOCKET_VOICECHAT_DISCARDED
};

class DccSocket : public QObject
{
	Q_OBJECT

	private:
		QSocketNotifier* snr;
		QSocketNotifier* snw;
		struct gg_dcc* dccsock;
		struct gg_event* dccevent;
		bool in_watchDcc;
		int State;
		static int Count;

	protected slots:
		void dccDataReceived();
		void dccDataSent();
	
	public:
		DccSocket(struct gg_dcc* dcc_sock);
		~DccSocket();
		struct gg_dcc* ggDccStruct();
		struct gg_event* ggDccEvent();
		virtual void initializeNotifiers();
		virtual void watchDcc(int check);
		int state();
		static int count();
		virtual void setState(int pstate);

	signals:
		void dccFinished(DccSocket* dcc);
};

class DccManager : public QObject
{
	Q_OBJECT

	private:
		friend class DccSocket;
		gg_dcc* DccSock;
		QSocketNotifier* DccSnr;
		QSocketNotifier* DccSnw;
		QHostAddress ConfigDccIp;
		QTimer TimeoutTimer;
		void watchDcc();

	private slots:
		void setupDcc();
		void closeDcc();
		/**
			Otrzymano wiadomo¶æ CTCP.
			Kto¶ nas prosi o po³±czenie dcc, poniewa¿
			jeste¶my za NAT-em.
		**/
		void dccConnectionReceived(const UserListElement& sender);
		void timeout();

	public:
		DccManager();
		virtual ~DccManager();
		QHostAddress configDccIp();
		void startTimeout();
		void cancelTimeout();

	public slots:
		void dccFinished(DccSocket* dcc);
		void dccReceived();
		void dccSent();
		void ifDccEnabled(bool value);
		void ifDccIpEnabled(bool value);
		void configDialogCreated();
		void configDialogApply();

	signals:
		void dccEvent(DccSocket* socket);
		void connectionBroken(DccSocket* socket);
		void dccError(DccSocket* socket);
		void needFileAccept(DccSocket* socket);
		void needFileInfo(DccSocket* socket);
		void noneEvent(DccSocket* socket);
		void dccDone(DccSocket* socket);
		void callbackReceived(DccSocket* socket);
		void setState(DccSocket* socket);
		void socketDestroying(DccSocket* socket);
};

extern DccManager* dcc_manager;

#endif
