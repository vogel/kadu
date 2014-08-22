/*
 * %kadu copyright begin%
 * Copyright 2008, 2009, 2010, 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009, 2009, 2012 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2008, 2010 Tomasz Rostański (rozteck@interia.pl)
 * Copyright 2011 Piotr Dąbrowski (ultr@ultr.pl)
 * Copyright 2004 Tomasz Jarzynka (tomee@cpi.pl)
 * Copyright 2004, 2008, 2009 Michał Podsiadlik (michal@kadu.net)
 * Copyright 2009 Bartłomiej Zimoń (uzi18@o2.pl)
 * Copyright 2004 Roman Krzystyniak (Ron_K@tlen.pl)
 * Copyright 2002, 2003, 2004, 2007 Adrian Smarzewski (adrian@kadu.net)
 * Copyright 2003, 2004, 2005 Paweł Płuciennik (pawel_p@kadu.net)
 * Copyright 2002, 2003, 2004 Tomasz Chiliński (chilek@chilan.com)
 * Copyright 2007, 2008, 2009, 2009, 2010, 2011, 2012, 2013, 2014 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2010, 2011, 2012, 2013 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2006, 2008 Dawid Stawiarski (neeo@kadu.net)
 * Copyright 2004, 2005, 2006, 2007 Marcin Ślusarz (joi@kadu.net)
 * Copyright 2003 Dariusz Jagodzik (mast3r@kadu.net)
 * Copyright 2009 Longer (longer89@gmail.com)
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
#include "core/application.h"
#include "core/core.h"
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

History * History::Instance = 0;

void History::createInstance()
{
	if (!Instance)
	{
		Instance = new History();
		Instance->setChatWidgetRepository(Core::instance()->chatWidgetRepository());
	}
}

void History::destroyInstance()
{
	delete Instance;
	Instance = 0;
}

History * History::instance()
{
	return Instance;
}

History::History() :
		ConfigurationUiHandler(0), SyncEnabled(true), SaveThread(0), CurrentStorage(0)
{
	kdebugf();
	createActionDescriptions();
	connect(AccountManager::instance(), SIGNAL(accountRegistered(Account)),
		this, SLOT(accountRegistered(Account)));
	connect(AccountManager::instance(), SIGNAL(accountUnregistered(Account)),
		this, SLOT(accountUnregistered(Account)));
	connect(MessageManager::instance(), SIGNAL(messageReceived(Message)),
		this, SLOT(enqueueMessage(Message)));
	connect(MessageManager::instance(), SIGNAL(messageSent(Message)),
		this, SLOT(enqueueMessage(Message)));

	createDefaultConfiguration();
	configurationUpdated();
	kdebugf2();
}

History::~History()
{
	kdebugf();

	disconnect(MessageManager::instance(), 0, this, 0);

	stopSaveThread();
	deleteActionDescriptions();

	kdebugf2();
}

void History::setChatWidgetRepository(ChatWidgetRepository *chatWidgetRepository)
{
	m_chatWidgetRepository = chatWidgetRepository;

	if (m_chatWidgetRepository)
		connect(m_chatWidgetRepository.data(), SIGNAL(chatWidgetAdded(ChatWidget *)), this, SLOT(chatWidgetAdded(ChatWidget *)));
}

void History::createActionDescriptions()
{
	Actions::instance()->blockSignals();

	ShowHistoryActionDescriptionInstance = new ShowHistoryActionDescription(this);

	MenuInventory::instance()
		->menu("buddy-list")
		->addAction(ShowHistoryActionDescriptionInstance, KaduMenu::SectionView, 100)
		->update();
	MenuInventory::instance()
		->menu("main")
		->addAction(ShowHistoryActionDescriptionInstance, KaduMenu::SectionRecentChats)
		->update();

	// The last ActionDescription will send actionLoaded() signal.
	Actions::instance()->unblockSignals();

	ClearHistoryActionDescription = new ActionDescription(this,
		ActionDescription::TypeUser, "clearHistoryAction",
		this, SLOT(clearHistoryActionActivated(QAction *, bool)),
		KaduIcon("kadu_icons/clear-history"), tr("Clear History"), false,
		disableNonHistoryContacts
	);

}

void History::deleteActionDescriptions()
{
	MenuInventory::instance()
		->menu("buddy-list")
		->removeAction(ShowHistoryActionDescriptionInstance)
		->update();
	MenuInventory::instance()
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

	Chat chat = BuddyChatManager::instance()->buddyChat(chatWidget->chat());

	HistoryQuery query;
	query.setTalkable(chat ? chat : chatWidget->chat());
	query.setFromDateTime(QDateTime::currentDateTime().addSecs(ChatHistoryQuotationTime * 3600));
	query.setLimit(Application::instance()->configuration()->deprecatedApi()->readNumEntry("History", "ChatHistoryCitation", 10));

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

void History::mainConfigurationWindowCreated(MainConfigurationWindow *mainConfigurationWindow)
{
	Q_UNUSED(mainConfigurationWindow)
}

void History::configurationUpdated()
{
	kdebugf();

	ChatHistoryCitation = Application::instance()->configuration()->deprecatedApi()->readNumEntry("History", "ChatHistoryCitation");
	ChatHistoryQuotationTime = Application::instance()->configuration()->deprecatedApi()->readNumEntry("History", "ChatHistoryQuotationTime", -24);

	SaveChats = Application::instance()->configuration()->deprecatedApi()->readBoolEntry("History", "SaveChats", true);
	SaveChatsWithAnonymous = Application::instance()->configuration()->deprecatedApi()->readBoolEntry("History", "SaveChatsWithAnonymous", true);
	SaveStatuses = Application::instance()->configuration()->deprecatedApi()->readBoolEntry("History", "SaveStatusChanges", false);
	SaveOnlyStatusesWithDescription = Application::instance()->configuration()->deprecatedApi()->readBoolEntry("History", "SaveOnlyStatusWithDescription", false);

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

	foreach (const Account &account, AccountManager::instance()->items())
		accountRegistered(account);

	emit storageChanged(CurrentStorage);
}

void History::unregisterStorage(HistoryStorage *storage)
{
	if (CurrentStorage != storage)
		return;

	foreach (const Account &account, AccountManager::instance()->items())
		accountUnregistered(account);

	stopSaveThread();

	delete CurrentStorage;
	CurrentStorage = 0;

	emit storageChanged(CurrentStorage);
}

void History::createDefaultConfiguration()
{
	Application::instance()->configuration()->deprecatedApi()->addVariable("History", "SaveChats", Application::instance()->configuration()->deprecatedApi()->readBoolEntry("History", "Logging", true));
	Application::instance()->configuration()->deprecatedApi()->removeVariable("History", "Logging");

	Application::instance()->configuration()->deprecatedApi()->addVariable("ShortCuts", "kadu_viewhistory", "Ctrl+H");

	Application::instance()->configuration()->deprecatedApi()->addVariable("History", "SaveStatusChanges", true);

	Application::instance()->configuration()->deprecatedApi()->addVariable("History", "SaveChatsWithAnonymous", true);

	Application::instance()->configuration()->deprecatedApi()->addVariable("History", "SaveOnlyStatusWithDescription", true);

	Application::instance()->configuration()->deprecatedApi()->addVariable("History", "ChatHistoryCitation", 10);
	Application::instance()->configuration()->deprecatedApi()->addVariable("History", "ChatHistoryQuotationTime", -24);
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
