/*
 * %kadu copyright begin%
 * Copyright 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2012 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2011, 2012, 2013, 2014 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#include <QtCore/QList>
#include <QtCore/QMutex>
#include <QtGui/QKeyEvent>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QMenu>
#include <QtWidgets/QPushButton>

#include "accounts/account-manager.h"
#include "accounts/account.h"
#include "buddies/buddy-manager.h"
#include "buddies/buddy.h"
#include "chat/buddy-chat-manager.h"
#include "chat/chat-manager.h"
#include "chat/chat.h"
#include "configuration/configuration.h"
#include "configuration/deprecated-configuration-api.h"
#include "contacts/contact-set.h"
#include "core/core.h"
#include "core/core.h"
#include "core/injected-factory.h"
#include "gui/actions/actions.h"
#include "gui/menu/menu-inventory.h"
#include "gui/widgets/chat-edit-box.h"
#include "gui/widgets/chat-widget/chat-widget-repository.h"
#include "gui/widgets/chat-widget/chat-widget.h"
#include "gui/widgets/configuration/config-group-box.h"
#include "gui/widgets/configuration/configuration-widget.h"
#include "gui/windows/message-dialog.h"
#include "message/message-manager.h"
#include "message/message.h"
#include "message/sorted-messages.h"
#include "protocols/services/chat-service.h"
#include "debug.h"

#include "actions/show-history-action-description.h"
#include "gui/windows/history-window.h"
#include "history-messages-prepender.h"
#include "history-query.h"
#include "history-save-thread.h"

#include "history.h"

void disableNonHistoryContacts(Action *action)
{
	kdebugf();
	action->setEnabled(false);
	const ContactSet &contacts = action->context()->contacts();

	if (contacts.isEmpty())
		return;

	foreach (const Contact &contact, contacts)
	{
		if (Core::instance()->myself() == contact.ownerBuddy())
			return;

		Account account = contact.contactAccount();
		if (!account.protocolHandler() || !account.protocolHandler()->chatService())
			return;
	}

	action->setEnabled(true);
	kdebugf2();
}

History::History(QObject *parent) :
		QObject{parent},
		SyncEnabled(true), SaveThread(0), CurrentStorage(0)
{
}

History::~History()
{
}

void History::setAccountManager(AccountManager *accountManager)
{
	m_accountManager = accountManager;
}

void History::setActions(Actions *actions)
{
	m_actions = actions;
}

void History::setChatWidgetRepository(ChatWidgetRepository *chatWidgetRepository)
{
	m_chatWidgetRepository = chatWidgetRepository;
}

void History::setConfiguration(Configuration *configuration)
{
	m_configuration = configuration;
}

void History::setInjectedFactory(InjectedFactory *injectedFactory)
{
	m_injectedFactory = injectedFactory;
}

void History::setMenuInventory(MenuInventory *menuInventory)
{
	m_menuInventory = menuInventory;
}

void History::setMessageManager(MessageManager* messageManager)
{
	connect(messageManager, SIGNAL(messageReceived(Message)), this, SLOT(enqueueMessage(Message)));
	connect(messageManager, SIGNAL(messageSent(Message)), this, SLOT(enqueueMessage(Message)));
}

void History::init()
{
	createActionDescriptions();
	connect(m_accountManager, SIGNAL(accountRegistered(Account)), this, SLOT(accountRegistered(Account)));
	connect(m_accountManager, SIGNAL(accountUnregistered(Account)), this, SLOT(accountUnregistered(Account)));
	connect(m_chatWidgetRepository, SIGNAL(chatWidgetAdded(ChatWidget *)), this, SLOT(chatWidgetAdded(ChatWidget *)));

	createDefaultConfiguration();
	configurationUpdated();
}

void History::done()
{
	stopSaveThread();
	deleteActionDescriptions();
}

void History::createActionDescriptions()
{
	m_actions->blockSignals();

	ShowHistoryActionDescriptionInstance = new ShowHistoryActionDescription(m_injectedFactory, this, this);

	m_menuInventory
		->menu("buddy-list")
		->addAction(ShowHistoryActionDescriptionInstance, KaduMenu::SectionView, 100)
		->update();
	m_menuInventory
		->menu("main")
		->addAction(ShowHistoryActionDescriptionInstance, KaduMenu::SectionRecentChats)
		->update();

	// The last ActionDescription will send actionLoaded() signal.
	m_actions->unblockSignals();

	ClearHistoryActionDescription = new ActionDescription(this,
		ActionDescription::TypeUser, "clearHistoryAction",
		this, SLOT(clearHistoryActionActivated(QAction *, bool)),
		KaduIcon("kadu_icons/clear-history"), tr("Clear History"), false,
		disableNonHistoryContacts
	);

}

void History::deleteActionDescriptions()
{
	m_menuInventory
		->menu("buddy-list")
		->removeAction(ShowHistoryActionDescriptionInstance)
		->update();
	m_menuInventory
		->menu("main")
		->removeAction(ShowHistoryActionDescriptionInstance)
		->update();

	delete ShowHistoryActionDescriptionInstance;
	ShowHistoryActionDescriptionInstance = 0;
}

void History::clearHistoryActionActivated(QAction *sender, bool toggled)
{
	Q_UNUSED(toggled)

	if (!CurrentStorage)
		return;

	Action *action = qobject_cast<Action *>(sender);
	if (!action)
		return;

	if (action->context()->chat())
		CurrentStorage->clearChatHistory(action->context()->chat());
}

void History::chatWidgetAdded(ChatWidget *chatWidget)
{
	kdebugf();

	if (!chatWidget)
		return;

	if (!CurrentStorage)
		return;

	WebkitMessagesView *chatMessagesView = chatWidget->chatMessagesView();
	if (!chatMessagesView)
		return;

	Chat chat = Core::instance()->buddyChatManager()->buddyChat(chatWidget->chat());

	HistoryQuery query;
	query.setTalkable(chat ? chat : chatWidget->chat());
	query.setFromDateTime(QDateTime::currentDateTime().addSecs(ChatHistoryQuotationTime * 3600));
	query.setLimit(m_configuration->deprecatedApi()->readNumEntry("History", "ChatHistoryCitation", 10));

	new HistoryMessagesPrepender(CurrentStorage->messages(query), chatMessagesView);
}

void History::accountRegistered(Account account)
{
	if (!account.protocolHandler())
		return;

	connect(account, SIGNAL(buddyStatusChanged(Contact, Status)),
			this, SLOT(contactStatusChanged(Contact, Status)));
}

void History::accountUnregistered(Account account)
{
	disconnect(account, 0, this, 0);

	if (!account.protocolHandler())
		return;

	ChatService *service = account.protocolHandler()->chatService();
	if (service)
		disconnect(service, 0, this, 0);
}

bool History::shouldSaveForBuddy(const Buddy &buddy)
{
	if (!buddy)
		return false;

	return buddy.property("history:StoreHistory", true).toBool();
}

bool History::shouldSaveForChat(const Chat &chat)
{
	if (!chat)
		return false;

	return chat.property("history:StoreHistory", true).toBool();
}

bool History::shouldEnqueueMessage(const Message &message)
{
	if (!SaveChats)
		return false;

	const int contactCount = message.messageChat().contacts().count();
	const Contact &contact = message.messageChat().contacts().toContact();

	if (!SaveChatsWithAnonymous && 1 == contactCount && contact.isAnonymous())
		return false;

	if (1 == contactCount)
		return shouldSaveForBuddy(contact.ownerBuddy());
	else
		return shouldSaveForChat(message.messageChat());
}

void History::enqueueMessage(const Message &message)
{
	if (!CurrentStorage || !shouldEnqueueMessage(message))
		return;

	UnsavedDataMutex.lock();
	UnsavedMessages.enqueue(message);
	UnsavedDataMutex.unlock();

	SaveThread->newDataAvailable();
}

void History::contactStatusChanged(Contact contact, Status oldStatus)
{
	Q_UNUSED(oldStatus)

	Status status = contact.currentStatus();
	if (!CurrentStorage || !SaveStatuses)
		return;

	if (SaveOnlyStatusesWithDescription && status.description().isEmpty())
		return;

	if (!shouldSaveForBuddy(contact.ownerBuddy()))
		return;

	UnsavedDataMutex.lock();
	UnsavedStatusChanges.enqueue(qMakePair(contact, status));
	UnsavedDataMutex.unlock();

	SaveThread->newDataAvailable();
}

Message History::dequeueUnsavedMessage()
{
	QMutexLocker locker(&UnsavedDataMutex);

	if (UnsavedMessages.isEmpty())
		return Message::null;

	return UnsavedMessages.dequeue();
}

QPair<Contact, Status> History::dequeueUnsavedStatusChange()
{
	QMutexLocker locker(&UnsavedDataMutex);

	if (UnsavedStatusChanges.isEmpty())
		return qMakePair(Contact::null, Status());

	return UnsavedStatusChanges.dequeue();
}

void History::crash()
{
	// don't try to stop/wait on thread here, see bug #2228
	if (SaveThread)
		SaveThread->forceSync(true);
}

void History::startSaveThread()
{
	if (!SaveThread)
	{
		SaveThread = new HistorySaveThread(this, this);
		SaveThread->setEnabled(SyncEnabled);
	}

	if (!SaveThread->isRunning())
		SaveThread->start();
}

void History::stopSaveThread()
{
	if (SaveThread && SaveThread->isRunning())
	{
		SaveThread->stop();
		SaveThread->wait(30000);

		if (SaveThread->isRunning())
		{
			kdebugm(KDEBUG_WARNING, "terminating history save thread!\n");

			SaveThread->terminate();
			SaveThread->wait(2000);
		}
	}
}

void History::configurationUpdated()
{
	kdebugf();

	ChatHistoryCitation = m_configuration->deprecatedApi()->readNumEntry("History", "ChatHistoryCitation");
	ChatHistoryQuotationTime = m_configuration->deprecatedApi()->readNumEntry("History", "ChatHistoryQuotationTime", -24);

	SaveChats = m_configuration->deprecatedApi()->readBoolEntry("History", "SaveChats", true);
	SaveChatsWithAnonymous = m_configuration->deprecatedApi()->readBoolEntry("History", "SaveChatsWithAnonymous", true);
	SaveStatuses = m_configuration->deprecatedApi()->readBoolEntry("History", "SaveStatusChanges", false);
	SaveOnlyStatusesWithDescription = m_configuration->deprecatedApi()->readBoolEntry("History", "SaveOnlyStatusWithDescription", false);

	kdebugf2();
}

void History::registerStorage(HistoryStorage *storage)
{
	CurrentStorage = storage;

	stopSaveThread();

	if (!CurrentStorage)
		return;

	startSaveThread();

	if (m_chatWidgetRepository)
		for (ChatWidget *chat : m_chatWidgetRepository.data())
			chatWidgetAdded(chat);

	foreach (const Account &account, m_accountManager->items())
		accountRegistered(account);

	emit storageChanged(CurrentStorage);
}

void History::unregisterStorage(HistoryStorage *storage)
{
	if (CurrentStorage != storage)
		return;

	foreach (const Account &account, m_accountManager->items())
		accountUnregistered(account);

	stopSaveThread();

	CurrentStorage = 0;

	emit storageChanged(CurrentStorage);
}

void History::createDefaultConfiguration()
{
	m_configuration->deprecatedApi()->addVariable("History", "SaveChats", m_configuration->deprecatedApi()->readBoolEntry("History", "Logging", true));
	m_configuration->deprecatedApi()->removeVariable("History", "Logging");

	m_configuration->deprecatedApi()->addVariable("ShortCuts", "kadu_viewhistory", "Ctrl+H");

	m_configuration->deprecatedApi()->addVariable("History", "SaveStatusChanges", true);

	m_configuration->deprecatedApi()->addVariable("History", "SaveChatsWithAnonymous", true);

	m_configuration->deprecatedApi()->addVariable("History", "SaveOnlyStatusWithDescription", true);

	m_configuration->deprecatedApi()->addVariable("History", "ChatHistoryCitation", 10);
	m_configuration->deprecatedApi()->addVariable("History", "ChatHistoryQuotationTime", -24);
}

void History::forceSync()
{
	if (SaveThread)
		SaveThread->forceSync();
}

void History::setSyncEnabled(bool syncEnabled)
{
	if (SaveThread)
		SaveThread->setEnabled(syncEnabled);
}

#include "moc_history.cpp"
