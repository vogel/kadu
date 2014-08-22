/*
 * %kadu copyright begin%
 * Copyright 2008, 2010, 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2011 Piotr Dąbrowski (ultr@ultr.pl)
 * Copyright 2008 Michał Podsiadlik (michal@kadu.net)
 * Copyright 2007, 2008, 2009, 2010, 2011, 2012, 2013, 2014 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2011 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2007, 2008 Dawid Stawiarski (neeo@kadu.net)
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
	 * @enum State
	 * @short This enum represent state of chat.
	 * @note Values in this enum must be identical with those defined in kadu-core/chat-style/engine/chat-scripts.js
	 */
	enum State
	{
		/**
		 * Chat is active - window is opened and active.
		 */
		StateActive = 0,
		/**
		 * Chat is in compising state - new message is being entered, but not send yet.
		 */
		StateComposing = 1,
		/**
		 * Chat is gone - window was closed.
		 */
		StateGone = 2,
		/**
		 * Chat is inactive - window is opened but not active.
		 */
		StateInactive = 3,
		/**
		 * Unknown state.
		 */
		StateNone = 4,
		/**
		 * Chat is in paused state - new message is written, but entering was paused.
		 */
		StatePaused = 5
	};

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
	virtual void sendState(const Contact &contact, State state) = 0;

signals:
	/**
	 * @short Signal emited when peer changed its chat state.
	 * @param contact peer contact
	 * @param state new state received from peer
	 */
	void peerStateChanged(const Contact &contact, ChatStateService::State state);

};

/**
 * @}
 */

#endif // CHAT_STATE_SERVICE_H
