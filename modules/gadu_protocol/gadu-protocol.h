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

#include "conference/gadu-conference.h"
#include "services/gadu-chat-image-service.h"
#include "services/gadu-chat-service.h"
#include "services/gadu-contact-list-service.h"
#include "services/gadu-file-transfer-service.h"
#include "services/gadu-personal-info-service.h"
#include "services/gadu-search-service.h"

#include "gadu_exports.h"
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

private:
	GaduChatImageService *CurrentChatImageService;
	GaduChatService *CurrentChatService;
	GaduContactListService *CurrentContactListService;
	GaduFileTransferService *CurrentFileTransferService;
	GaduPersonalInfoService *CurrentPersonalInfoService;
	GaduSearchService *CurrentSearchService;

	friend class DccManager;
	DccManager *Dcc;

	QHostAddress ActiveServer;

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
	void networkDisconnected();

	void sendUserList();

	Status::StatusType statusTypeFromIndex(unsigned int index) const;

	void socketContactStatusChanged(unsigned int uin, unsigned int status, const QString &description,
			const QHostAddress &ip, unsigned short port, unsigned int maxImageSize, unsigned int version);
	void socketConnFailed(GaduError error);
	void socketConnSuccess();
	void socketDisconnected();
	void connectionBroken();

private slots:
	/**
		Loguje si� do serwera Gadu-Gadu. Po uruchomieniu emituje sygna� connecting. Parametry
		logowania odczytuje z konfiguracji, status logowania pobiera z pola NextStatus.
		��cz�c si�, wybiera kolejne serwery (w przypadku nieudanego po��czenia) wykorzystuj�c
		pola ConfigServers i i ServerNr.

		Po poprawnym zalogowaniu wywo�ywany jest slot connectedSlot, kt�ry emituje sygna�
		GaduProtocol::connected

		Metod� mo�na uruchomi� po�rednio poprzez wywo�anie typu gadu->status().setOnline(),
		kt�re wywo�a slot iWantToGoOnline, kt�ry z kolei (gdy stwierdzi, �e nie jeste�my zalogowani)
		wywo�a procedur�.

		@see connecting
		@see connected
		@see connectedSlot
		@see NextStatus
		@see ConfigServers
		@see ServerNr
	**/
	void login();

	/**
		Slot wywo�ywany po roz��czeniu z serwerem. Emituje disconnected i wy��cza pingowanie
		serwera.

		@see disconnected
	**/
	void disconnectedSlot();


	/**
		Slot wywo�ywany po przekroczeniu czasu po��czenia. Pr�buje po�aczy� ponownie.
	**/
	void connectionTimeoutTimerSlot();

	/**
		Wykonuje zadania co minut� - pinguje sie� i zeruje licznik
		odebranych obrazk�w (je�li jeste�my po��czeni).
	**/
	void everyMinuteActions();

protected:
	virtual void changeStatus(Status status);

public:
	static void initModule();

	static int statusToType(Status status);
	static Status typeToStatus(int type);

	GaduProtocol(Account *account, ProtocolFactory *factory);
	virtual ~GaduProtocol();

	virtual ChatImageService * chatImageService() { return CurrentChatImageService; }
	virtual ChatService * chatService() { return CurrentChatService; }
	virtual ContactListService * contactListService() { return CurrentContactListService; }
	virtual FileTransferService * fileTransferService() { return CurrentFileTransferService; }
	virtual PersonalInfoService * personalInfoService() { return CurrentPersonalInfoService; }
	virtual SearchService * searchService() { return CurrentSearchService; }

	virtual Conference * loadConferenceFromStorage(StoragePoint *storage);

	UinType uin(Contact contact) const;
	GaduContactAccountData * gaduContactAccountData(Contact contact) const;

	unsigned int maxDescriptionLength();

	gg_session * gaduSession() { return GaduSession; }
	GaduProtocolSocketNotifiers * socketNotifiers() { return SocketNotifiers; }
	DccManager * dccManager() { return Dcc; }

	void changeID(const QString &id);
	/**
		Zwraca serwer z kt�rym jeste�my po��czeni lub do kt�rego si� w�a�nie ��czymy.
		isNull() = hub.
	**/
	QHostAddress activeServer();

	/**
		Po jedno sekundowym op��nieniu wykonuje pr�b� po��czenia.
	**/
	void connectAfterOneSecond();

	bool validateUserID(QString &uid);
	GaduConference * conference(ContactList contacts);

	virtual QPixmap statusPixmap(Status status);

signals:
	/**
		Served sent information about status change for unknown user.
	**/
	void userStatusChangeIgnored(Contact);

};

#endif // GADU_PROTOCOL_H
