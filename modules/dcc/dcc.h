#ifndef KADU_DCC_H
#define KADU_DCC_H

#include <qhostaddress.h>
#include <qtimer.h>

#include "kadu-config.h"
#include "gadu.h"

class QSocketNotifier;

/**
 * @defgroup dcc Dcc
 * @{
 */
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
		QSocketNotifier* readSocketNotifier;
		QSocketNotifier* writeSocketNotifier;
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
		struct gg_dcc* ggDccStruct() const;
		struct gg_event* ggDccEvent() const;
		virtual void initializeNotifiers();
		virtual void watchDcc(int check);
		int state() const;
		static int count();
		virtual void setState(int pstate);

		void acceptFile();
		void discardFile();

	signals:
		void dccFinished(DccSocket* dcc);
};

class DccManager : public QObject
{
	Q_OBJECT

	private:
		friend class DccSocket;
		gg_dcc* DccSock;
		QSocketNotifier* DCCReadSocketNotifier;
		QSocketNotifier* DCCWriteSocketNotifier;

		QTimer TimeoutTimer;
		void watchDcc();
		QMap<UinType, int> requests;
		bool DccEnabled;

	private slots:
		void startTimeout();
		void cancelTimeout();
		void setupDcc();
		void closeDcc();

		/**
			Otrzymano wiadomo¶æ CTCP.
			Kto¶ nas prosi o po³±czenie dcc, poniewa¿
			jeste¶my za NAT-em.
		**/
		void dccConnectionReceived(const UserListElement& sender);
		void timeout();
		void callbackReceived(DccSocket *socket);

		void dccFinished(DccSocket* dcc);
		void dccReceived();
		void dccSent();
		void ifDccEnabled(bool value);
		void ifDccIpEnabled(bool value);
		void configDialogCreated();
		void configDialogApply();

	public:
		DccManager(QObject *parent=0, const char *name=0);
		virtual ~DccManager();
		enum TryType {DIRECT, REQUEST};
		TryType initDCCConnection(uint32_t ip, uint16_t port,
								UinType my_uin, UinType peer_uin, const char *gadu_slot,
								int dcc_type, bool force_request=false);
		bool dccEnabled() const;

	signals:
		void dccEvent(DccSocket* socket);
		void connectionBroken(DccSocket* socket);
		void dccError(DccSocket* socket);
		void needFileAccept(DccSocket* socket);
		void needFileInfo(DccSocket* socket);
		void noneEvent(DccSocket* socket);
		void dccDone(DccSocket* socket);
		void setState(DccSocket* socket);
		void socketDestroying(DccSocket* socket);

		/* nie dotykaæ */
		void dccSig(uint32_t ip, uint16_t port, UinType my_uin, UinType peer_uin, struct gg_dcc **out);
};

extern DccManager* dcc_manager;

/** @} */

#endif
