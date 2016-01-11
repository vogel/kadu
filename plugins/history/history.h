/*
 * %kadu copyright begin%
 * Copyright 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2012 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2011, 2014 Bartosz Brachaczek (b.brachaczek@gmail.com)
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
#include <QtCore/QMap>
#include <QtCore/QMutex>
#include <QtCore/QObject>
#include <QtCore/QPair>
#include <QtCore/QQueue>
#include <QtCore/QString>
#include <QtCore/QStringList>
#include <QtCore/QVariant>
#include <QtGui/QKeyEvent>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QDialog>
#include <QtWidgets/QLabel>
#include <QtWidgets/QListWidget>
#include <injeqt/injeqt.h>

#include "configuration/configuration-aware-object.h"
#include "configuration/gui/configuration-ui-handler.h"
#include "core/crash-aware-object.h"
#include "gui/actions/action-description.h"
#include "gui/actions/action.h"
#include "message/message.h"
#include "protocols/protocol.h"
#include "storage/history-storage.h"

#include "history-exports.h"

class AccountManager;
class Account;
class Actions;
class ChatWidget;
class ChatWidgetRepository;
class Configuration;
class HistorySaveThread;
class HistoryWindow;
class InjectedFactory;
class MessageManager;
class ShowHistoryActionDescription;

class HISTORYAPI History : public QObject, ConfigurationAwareObject, CrashAwareObject
{
	Q_OBJECT

	QPointer<AccountManager> m_accountManager;
	QPointer<Actions> m_actions;
	QPointer<ChatWidgetRepository> m_chatWidgetRepository;
	QPointer<Configuration> m_configuration;
	QPointer<InjectedFactory> m_injectedFactory;

	bool SaveChats;
	bool SaveChatsWithAnonymous;
	bool SaveStatuses;
	bool SaveOnlyStatusesWithDescription;
	bool SyncEnabled;

	int ChatHistoryCitation;
	int ChatHistoryQuotationTime;

	QMutex UnsavedDataMutex;
	QQueue<Message> UnsavedMessages;
	QQueue<QPair<Contact, Status> > UnsavedStatusChanges;
	HistorySaveThread *SaveThread;

	HistoryStorage *CurrentStorage;

	ShowHistoryActionDescription *ShowHistoryActionDescriptionInstance;
	ActionDescription *ClearHistoryActionDescription;

	QListWidget *allStatusUsers;
	QListWidget *selectedStatusUsers;
	QListWidget *allChatsUsers;
	QListWidget *selectedChatsUsers;

	void startSaveThread();
	void stopSaveThread();

	void createDefaultConfiguration();

	void createActionDescriptions();
	void deleteActionDescriptions();
	virtual void configurationUpdated();

	friend class HistorySaveThread;
	Message dequeueUnsavedMessage();
	QPair<Contact, Status> dequeueUnsavedStatusChange();

	bool shouldSaveForBuddy(const Buddy &buddy);
	bool shouldSaveForChat(const Chat &chat);
	bool shouldEnqueueMessage(const Message &message);

private slots:
	INJEQT_SET void setAccountManager(AccountManager *accountManager);
	INJEQT_SET void setActions(Actions *actions);
	INJEQT_SET void setChatWidgetRepository(ChatWidgetRepository *chatWidgetRepository);
	INJEQT_SET void setConfiguration(Configuration *configuration);
	INJEQT_SET void setInjectedFactory(InjectedFactory *injectedFactory);
	INJEQT_SET void setMessageManager(MessageManager *messageManager);
	INJEQT_INIT void init();

	void accountRegistered(Account);
	void accountUnregistered(Account);

	void enqueueMessage(const Message &);
	void contactStatusChanged(Contact contact, Status oldStatus);

	void clearHistoryActionActivated(QAction *sender, bool toggled);

	void chatWidgetAdded(ChatWidget *chatWidget);

protected:
	virtual void crash();

public:
	Q_INVOKABLE explicit History(QObject *parent = nullptr);
	virtual ~History();

	HistoryStorage * currentStorage() { return CurrentStorage; }
	void registerStorage(HistoryStorage *storage);
	void unregisterStorage(HistoryStorage *storage);

	void forceSync();
	void setSyncEnabled(bool syncEnabled);

signals:
	void storageChanged(HistoryStorage *newStorage);

};

void disableNonHistoryContacts(Action *action);
