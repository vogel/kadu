/*
 * %kadu copyright begin%
 * Copyright 2014 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#pragma once

#include <QtCore/QDateTime>
#include <QtCore/QObject>
#include <QtCore/QPointer>
#include <QtCore/QRegExp>
#include <injeqt/injeqt.h>

#include "chat/chat-manager.h"
#include "chat/chat.h"
#include "contacts/contact-set.h"

#include "accounts/accounts-aware-object.h"
#include "configuration/configuration-aware-object.h"
#include "message/message-filter.h"

class AccountManager;
class Account;
class BuddyManager;
class ChatManager;
class ChatStorage;
class ChatWidget;
class ChatWidgetRepository;
class Configuration;
class Contact;
class FirewallNotificationService;
class History;
class IncomingMessageFirewallFilter;
class PluginInjectedFactory;
class KaduWindowService;
class MessageManager;
class MessageStorage;
class Message;
class OutgoingMessageFirewallFilter;
class PathsProvider;

class FirewallMessageFilter : public QObject, public MessageFilter, ConfigurationAwareObject, AccountsAwareObject
{
	Q_OBJECT

public:
	Q_INVOKABLE explicit FirewallMessageFilter(QObject *parent = nullptr);
	virtual ~FirewallMessageFilter();

	virtual bool acceptMessage(const Message& message);

	bool acceptIncomingMessage(const Message &message);
	bool acceptOutgoingMessage(const Message &message);

private:
	QPointer<AccountManager> m_accountManager;
	QPointer<BuddyManager> m_buddyManager;
	QPointer<ChatManager> m_chatManager;
	QPointer<ChatStorage> m_chatStorage;
	QPointer<ChatWidgetRepository> m_chatWidgetRepository;
	QPointer<Configuration> m_configuration;
	QPointer<FirewallNotificationService> m_firewallNotificationService;
	QPointer<History> m_history;
	QPointer<PluginInjectedFactory> m_pluginInjectedFactory;
	QPointer<KaduWindowService> m_kaduWindowService;
	QPointer<MessageStorage> m_messageStorage;
	QPointer<MessageManager> m_messageManager;
	QPointer<PathsProvider> m_pathsProvider;

	BuddySet SecuredTemporaryAllowed;
	ContactSet Passed;
	Contact LastContact;

	unsigned int FloodMessages;
	QTime LastMsg;
	QTime LastNotify;
	QRegExp pattern;

	bool CheckFloodingEmoticons;
	bool EmoticonsAllowKnown;
	bool WriteLog;
	bool CheckDos;
	bool CheckChats;
	bool IgnoreConferences;
	bool WriteInHistory;
	bool DropAnonymousWhenInvisible;
	bool IgnoreInvisible;
	bool Confirmation;
	bool Search;
	bool SafeSending;
	int DosInterval;
	int MaxEmoticons;
	NormalizedHtmlString ConfirmationText;
	NormalizedHtmlString ConfirmationQuestion;
	QString LogFilePath;

	bool checkChat(const Chat &chat, const Contact &sender, const QString &message, bool &stop);
	bool checkConference(const Chat &chat);
	bool checkEmoticons(const QString &message);
	bool checkFlood();

	void writeLog(const Contact &contact, const QString &message);

	void createDefaultConfiguration();

private slots:
	INJEQT_SET void setAccountManager(AccountManager *accountManager);
	INJEQT_SET void setBuddyManager(BuddyManager *buddyManager);
	INJEQT_SET void setChatManager(ChatManager *chatManager);
	INJEQT_SET void setChatStorage(ChatStorage *chatStorage);
	INJEQT_SET void setChatWidgetRepository(ChatWidgetRepository *chatWidgetRepository);
	INJEQT_SET void setConfiguration(Configuration *configuration);
	INJEQT_SET void setFirewallNotificationService(FirewallNotificationService *firewallNotificationService);
	INJEQT_SET void setHistory(History *history);
	INJEQT_SET void setPluginInjectedFactory(PluginInjectedFactory *pluginInjectedFactory);
	INJEQT_SET void setKaduWindowService(KaduWindowService *kaduWindowService);
	INJEQT_SET void setMessageManager(MessageManager *messageManager);
	INJEQT_SET void setMessageStorage(MessageStorage *messageStorage);
	INJEQT_SET void setPathsProvider(PathsProvider *pathsProvider);
	INJEQT_INIT void init();
	INJEQT_DONE void done();

	void accountConnected();

	void chatDestroyed(ChatWidget *);

protected:
	virtual void accountRegistered(Account account);
	virtual void accountUnregistered(Account account);
	virtual void configurationUpdated();

};
