/*
 * %kadu copyright begin%
 * Copyright 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2011 Piotr Dąbrowski (ultr@ultr.pl)
 * Copyright 2011 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2011, 2012, 2013, 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * %kadu copyright end%
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef CHAT_STATE_SERVICE_H
#define CHAT_STATE_SERVICE_H

#include "protocols/services/account-service.h"

#include "exports.h"

class Chat;
class Contact;

enum class ChatState;

/**
 * @addtogroup Protocol
 * @{
 */

/**
 * @class ChatStateService
 * @short ChatStateService account service allows sending and receiving information about composing state in chats.
 *
 * This service allows sending and receiving information about composing state in chats. It supports several states
 * defined in ChatStateService::State enum.
 *
 * One method should be overrided by subclasses: sendState(). It sends our chat state to peer. When peer changes its
 * chats state, peerStateChanged() signal is emited.
 */
class KADUAPI ChatStateService : public AccountService
{
	Q_OBJECT

public:

	/**
	 * @short Create new instance of ChatStateService bound to given Account.
	 * @param account account to bound this service to
	 */
	explicit ChatStateService(Account account, QObject *parent = 0);
	virtual ~ChatStateService();

	/**
	 * @short Send our state to given contact.
	 * @param contact state of chat with this contact changed
	 * @param state new state to send
	 */
	virtual void sendState(const Contact &contact, ChatState state) = 0;

signals:
	/**
	 * @short Signal emited when peer changed its chat state.
	 * @param contact peer contact
	 * @param state new state received from peer
	 */
	void peerStateChanged(const Contact &contact, ChatState state);

};

/**
 * @}
 */

#endif // CHAT_STATE_SERVICE_H
