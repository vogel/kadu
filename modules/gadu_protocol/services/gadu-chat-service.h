/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef GADU_CHAT_SERVICE_H
#define GADU_CHAT_SERVICE_H

#include <libgadu.h>

#include "protocols/services/chat-service.h"

class GaduProtocol;

class GaduChatService : public ChatService
{
	Q_OBJECT

	GaduProtocol *Protocol;
	int LastMessageId;

private slots:
	void messageReceivedSlot(Contact sender, ContactList recipients, const QString &message, time_t time, QByteArray &formats);
	void ackReceived(int messageId, uin_t uin, int status);

public:
	GaduChatService(GaduProtocol *protocol);

public slots:
	virtual bool sendMessage(ContactList users, Message &message);

signals:
	/**
		Sygna� daje mozliwo�� operowania na wiadomo�ci
		kt�ra przysz�a z serwera jeszcze w jej oryginalnej
		formie przed konwersj� na unicode i innymi zabiegami.
		Tre�� wiadomo�ci mo�na zmieni� grzebi�c w buforze msg,
		ale uwaga: mo�na zepsu� formatowanie tekstu zapisane
		w formats. Oczywi�cie je r�wnie� mo�na zmienia� wed�ug
		opisu protoko�u GG ;)
		Mo�na te� przerwa� dalsz� obr�bk� wiadomo�ci ustawiaj�c
		stop na true.
	**/
	void rawGaduReceivedMessageFilter(Account *account, ContactList senders, QString &msg, QByteArray &formats, bool &ignore);

};

#endif // GADU_CHAT_SERVICE_H
