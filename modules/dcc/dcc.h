#ifndef KADU_DCC_H
#define KADU_DCC_H

#include <qhostaddress.h>
#include <qtimer.h>

#include "configuration_aware_object.h"
#include "gadu.h"
#include "main_configuration_window.h"

class QCheckBox;
class QSocketNotifier;
class QWidget;

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

	QSocketNotifier* readSocketNotifier;
	QSocketNotifier* writeSocketNotifier;
	struct gg_dcc* dccsock;
	struct gg_event* dccevent;
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
	void enableNotifiers();
	void disableNotifiers();

	virtual void watchDcc();
	int state() const;
	static int count();
	virtual void setState(int pstate);

	void discard();

signals:
	void dccFinished(DccSocket* dcc);
};

class DccManager : public ConfigurationUiHandler, ConfigurationAwareObject
{
	Q_OBJECT

	friend class DccSocket;
	gg_dcc* DccSock;
	QSocketNotifier* DCCReadSocketNotifier;
	QSocketNotifier* DCCWriteSocketNotifier;

	QTimer TimeoutTimer;
	void watchDcc();
	QMap<UinType, int> requests;
	bool DccEnabled;

	QWidget *ipAddress;
	QCheckBox *forwarding;
	QWidget *forwardingExternalIp;
	QWidget *forwardingExternalPort;
	QWidget *forwardingLocalPort;

	void createDefaultConfiguration();

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

	void onIpAutotetectToggled(bool toggled);

protected:
	virtual void configurationUpdated();

public:
	DccManager();
	virtual ~DccManager();
	enum TryType {DIRECT, REQUEST};
	TryType initDCCConnection(uint32_t ip, uint16_t port,
		UinType my_uin, UinType peer_uin, const char *gadu_slot,
		int dcc_type, bool force_request=false);
	bool dccEnabled() const;

	virtual void mainConfigurationWindowCreated(MainConfigurationWindow *mainConfigurationWindow);

signals:
	void dccEvent(DccSocket* socket, bool &lock);
	void connectionBroken(DccSocket* socket);
	void dccError(DccSocket* socket);
	void dccDone(DccSocket* socket);
	void setState(DccSocket* socket);
	void socketDestroying(DccSocket* socket);

	/* nie dotykaæ */
	void dccSig(uint32_t ip, uint16_t port, UinType my_uin, UinType peer_uin, struct gg_dcc **out);
};

extern DccManager* dcc_manager;

/** @} */

#endif
