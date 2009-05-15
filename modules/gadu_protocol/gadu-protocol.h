/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef GADU_PROTOCOL_H
#define GADU_PROTOCOL_H

#include <QtGui/QPixmap>
#include <QtNetwork/QHostAddress>

#include <libgadu.h>

#include "services/gadu-chat-image-service.h"
#include "services/gadu-chat-service.h"
#include "services/gadu-contact-list-service.h"
#include "services/gadu-file-transfer-service.h"
#include "services/gadu-personal-info-service.h"
#include "services/gadu-search-service.h"

#include "server/gadu-servers-manager.h"
#include "gadu-exports.h"
#include "gadu-search-record.h"
#include "gadu-search-result.h"

#include "protocols/protocol.h"

class DccManager;
class GaduContactAccountData;
class GaduProtocolSocketNotifiers;

class GADUAPI GaduProtocol : public Protocol
{
	Q_OBJECT
	Q_DISABLE_COPY(GaduProtocol);

public:
	enum GaduError
	{
		ConnectionServerNotFound,
		ConnectionCannotConnect,
		ConnectionNeedEmail,
		ConnectionInvalidData,
		ConnectionCannotRead,
		ConnectionCannotWrite,
		ConnectionIncorrectPassword,
		ConnectionTlsError,
		ConnectionIntruderError,
		ConnectionUnavailableError,
		ConnectionUnknow,
		ConnectionTimeout,
		Disconnected
	};

	typedef unsigned int UinType;

private:
	GaduChatImageService *CurrentChatImageService;
	GaduChatService *CurrentChatService;
	GaduContactListService *CurrentContactListService;
	GaduFileTransferService *CurrentFileTransferService;
	GaduPersonalInfoService *CurrentPersonalInfoService;
	GaduSearchService *CurrentSearchService;

	friend class DccManager;
	DccManager *Dcc;

	GaduServersManager::GaduServer ActiveServer;

	struct gg_login_params GaduLoginParams;
	gg_session *GaduSession;

	friend class GaduProtocolSocketNotifiers;
	GaduProtocolSocketNotifiers *SocketNotifiers;

	QTimer *PingTimer;

	void setupProxy();
	void setupDcc();
	void setupLoginParams();
	void cleanUpLoginParams();

	void networkConnected();
	void networkDisconnected(bool tryAgain);

	int notifyTypeFromContact(Contact &contact);
	void sendUserList();

	void socketContactStatusChanged(unsigned int uin, unsigned int status, const QString &description,
			const QHostAddress &ip, unsigned short port, unsigned int maxImageSize, unsigned int version);
	void socketConnFailed(GaduError error);
	void socketConnSuccess();
	void socketDisconnected();

private slots:
	void login();
	void connectionTimeoutTimerSlot();
	void everyMinuteActions();

	void contactAdded(Contact &contact);
	void contactRemoved(Contact &contact);
	void contactAccountDataAdded(Contact &contact, Account *contactAccount);
	void contactAccountDataAboutToBeRemoved(Contact &contact, Account *contactAccount);
	void contactAccountDataIdChanged(Contact &contact, Account *contactAccount, const QString &oldId);

protected:
	virtual void changeStatus();
	virtual void changePrivateMode();

public:
	static Status::StatusType statusTypeFromGaduStatus(unsigned int index);
	static unsigned int gaduStatusFromStatus(const Status &status);

	GaduProtocol(Account *account, ProtocolFactory *factory);
	virtual ~GaduProtocol();

	virtual ChatImageService * chatImageService() { return CurrentChatImageService; }
	virtual ChatService * chatService() { return CurrentChatService; }
	virtual ContactListService * contactListService() { return CurrentContactListService; }
	virtual FileTransferService * fileTransferService() { return CurrentFileTransferService; }
	virtual PersonalInfoService * personalInfoService() { return CurrentPersonalInfoService; }
	virtual SearchService * searchService() { return CurrentSearchService; }

	virtual bool validateUserID(QString &uid);
	virtual QPixmap statusPixmap(Status status);

	UinType uin(Contact contact) const;
	GaduContactAccountData * gaduContactAccountData(Contact contact) const;

	unsigned int maxDescriptionLength();

	gg_session * gaduSession() { return GaduSession; }
	GaduProtocolSocketNotifiers * socketNotifiers() { return SocketNotifiers; }
	DccManager * dccManager() { return Dcc; }

signals:
	/**
		Served sent information about status change for unknown user.
	**/
	void userStatusChangeIgnored(Contact);

};

#endif // GADU_PROTOCOL_H
