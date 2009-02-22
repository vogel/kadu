/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef DCC_MANAGER_H
#define DCC_MANAGER_H

#include <QtCore/QObject>
#include <QtCore/QTimer>

#include "protocols/protocol.h"

class DccSocketNotifiers;
class GaduContactAccountData;
class GaduFileTransfer;
class GaduProtocol;

enum DccVersion
{
	DccUnknow,
	Dcc6,
	Dcc7
};

class DccManager : public QObject
{
	Q_OBJECT

	GaduProtocol *Protocol;

	DccSocketNotifiers *MainSocketNotifiers;
	QList<DccSocketNotifiers *> SocketNotifiers;
	QList<GaduFileTransfer *> WaitingFileTransfers;

	bool DccEnabled;
/*
	QWidget *ipAddress;
	QCheckBox *forwarding;
	QWidget *forwardingExternalIp;
	QWidget *forwardingExternalPort;
	QWidget *forwardingLocalPort;*/

	void setUpDcc();
	void closeDcc();

	void connectSocketNotifiers(DccSocketNotifiers *notifiers);
	void disconnectSocketNotifiers(DccSocketNotifiers *notifiers);

	void createDefaultConfiguration();

	friend class DccSocketNotifiers;
	bool acceptConnection(unsigned int uin, unsigned int peerUin, unsigned int peerAddr);
	void needIncomingFileTransferAccept(DccSocketNotifiers *socket);

private slots:
	void dcc7New(struct gg_dcc7 *);
	void dcc7Rejected(struct gg_dcc7 *);
	void dcc7Error(struct gg_dcc7 *);

	void dccConnectionRequestReceived(Contact contact);

// 	void onIpAutotetectToggled(bool toggled);

	void socketNotifiersDestroyed(QObject *socketNotifiers);

	void dccIncomingConnection(struct gg_dcc *incomingConnection);
	void dccCallbackReceived(DccSocketNotifiers *socket);

	void attachSendFileTransferSocket6(unsigned int uin, GaduContactAccountData *gcad, GaduFileTransfer *gft);
	void attachSendFileTransferSocket7(unsigned int uin, GaduContactAccountData *gcad, GaduFileTransfer *gft);

protected:
	virtual void configurationUpdated();

public:
	DccManager(GaduProtocol *protocol);
	virtual ~DccManager();

	void attachSendFileTransferSocket(GaduFileTransfer *gft);

// 	void getFileTransferSocket(uint32_t ip, uint16_t port, UinType myUin, UinType peerUin, DccHandler *handler, bool request = false);
// 	void getVoiceSocket(uint32_t ip, uint16_t port, UinType myUin, UinType peerUin, DccHandler *handler, bool request = false);

	bool dccEnabled() const;

// 	virtual void mainConfigurationWindowCreated(MainConfigurationWindow *mainConfigurationWindow);

// signals:
// 	void socketDestroying(DccSocket* socket);
// 	void dcc7IncomingFileTransfer(DccSocket *socket);

};

#endif // DCC_MANAGER_H

// kate: indent-mode cstyle; replace-tabs off; tab-width 4; 
