/*
 * %kadu copyright begin%
 * Copyright 2010 Wojciech Treter (juzefwt@gmail.com)
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

#ifndef JABBER_CHAT_STATE_SERVICE_H
#define JABBER_CHAT_STATE_SERVICE_H

#include <QtCore/QTimer>

#include "xmpp_message.h"

#include "protocols/protocol.h"

class JabberProtocol;

class ChatState : public QObject
{
	Q_OBJECT
	
	Chat ObservedChat;
	JabberProtocol *Protocol;
	// Message Events & Chat States
	QTimer* composingTimer_;
	bool isComposing_;
	bool sendComposingEvents_;
	QString eventId_;
	XMPP::ChatState contactChatState_;
	XMPP::ChatState lastChatState_;
	
	void setContactChatState(XMPP::ChatState state);

  public:
	ChatState(Chat chat);
	
  private slots:
  	void setComposing();
  	void resetComposing();
	void checkComposing();
	void updateIsComposing(bool b);
	void setChatState(XMPP::ChatState state);
	void incomingMessage(const XMPP::Message &m);
	
  signals:
    	/**
	 * Signals if user (re)started/stopped composing
	 */
	void composing(bool);
public slots:
    void incomingMess(XMPP::Message);
};

class JabberChatStateService : public QObject
{
	Q_OBJECT
	
	JabberProtocol *ParentProtocol;
	QList<ChatState *> ChatStateList;

  public:
	JabberChatStateService(JabberProtocol *parent);

};

#endif // JABBER_CHAT_STATE_SERVICE_H
