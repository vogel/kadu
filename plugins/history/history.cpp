/*
 * %kadu copyright begin%
 * Copyright 2011 Piotr Dąbrowski (ultr@ultr.pl)
 * Copyright 2006, 2008 Dawid Stawiarski (neeo@kadu.net)
 * Copyright 2004 Tomasz Jarzynka (tomee@cpi.pl)
 * Copyright 2010, 2011 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2008, 2009, 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2004, 2005, 2006, 2007 Marcin Ślusarz (joi@kadu.net)
 * Copyright 2002, 2003, 2004, 2007 Adrian Smarzewski (adrian@kadu.net)
 * Copyright 2002, 2003, 2004 Tomasz Chiliński (chilek@chilan.com)
 * Copyright 2007, 2008, 2009, 2010, 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2004 Roman Krzystyniak (Ron_K@tlen.pl)
 * Copyright 2004, 2008, 2009 Michał Podsiadlik (michal@kadu.net)
 * Copyright 2009 Longer (longer89@gmail.com)
 * Copyright 2008, 2010 Tomasz Rostański (rozteck@interia.pl)
 * Copyright 2009 Bartłomiej Zimoń (uzi18@o2.pl)
 * Copyright 2003, 2004, 2005 Paweł Płuciennik (pawel_p@kadu.net)
 * Copyright 2003 Dariusz Jagodzik (mast3r@kadu.net)
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
#include <QtCore/QScopedPointer>
#include <QtGui/QKeyEvent>
#include <QtGui/QLabel>
#include <QtGui/QGridLayout>
#include <QtGui/QMenu>
#include <QtGui/QPushButton>

#include "accounts/account.h"
#include "accounts/account-manager.h"
#include "buddies/buddy-manager.h"
#include "buddies/buddy-shared.h"
#include "chat/aggregate-chat-manager.h"
#include "chat/chat.h"
#include "chat/chat-manager.h"
#include "chat/message/message.h"
#include "chat/message/pending-messages-manager.h"
#include "configuration/configuration-file.h"
#include "buddies/buddy.h"
#include "contacts/contact-set.h"
#include "core/core.h"
#include "gui/widgets/configuration/config-group-box.h"
#include "gui/widgets/configuration/configuration-widget.h"
#include "gui/widgets/chat-edit-box.h"
#include "gui/widgets/chat-widget.h"
#include "gui/widgets/chat-widget-manager.h"
#include "gui/windows/kadu-window.h"
#include "gui/windows/message-dialog.h"
#include "misc/path-conversion.h"
#include "protocols/services/chat-service.h"

#include "debug.h"

#include "gui/windows/history-window.h"
#include "model/dates-model-item.h"
#include "history-save-thread.h"
#include "timed-status.h"

#include "history.h"

void disableNonHistoryContacts(Action *action)
{
	kdebugf();
	action->setEnabled(false);
	ContactSet contacts = action->contacts();

	if (!contacts.count())
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
		Instance = new History();
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

	connect(ChatWidgetManager::instance(), SIGNAL(chatWidgetCreated(ChatWidget *)), this, SLOT(chatCreated(ChatWidget *)));

	createDefaultConfiguration();
	configurationUpdated();
	kdebugf2();
}

History::~History()
{
	kdebugf();
	stopSaveThread();
	deleteActionDescriptions();

	kdebugf2();
}

void History::createActionDescriptions()
{
	ShowHistoryActionDescription = new ActionDescription(this,
		ActionDescription::TypeUser, "showHistoryAction",
		this, SLOT(showHistoryActionActivated(QAction *, bool)),
		KaduIcon("kadu_icons/history"), tr("View Chat History"), false
	);
	ShowHistoryActionDescription->setShortcut("kadu_viewhistory");
	BuddiesListViewMenuManager::instance()->addActionDescription(ShowHistoryActionDescription, BuddiesListViewMenuItem::MenuCategoryView, 100);
	Core::instance()->kaduWindow()->insertMenuActionDescription(ShowHistoryActionDescription, KaduWindow::MenuKadu, 5);

	ClearHistoryActionDescription = new ActionDescription(this,
		ActionDescription::TypeUser, "clearHistoryAction",
		this, SLOT(clearHistoryActionActivated(QAction *, bool)),
		KaduIcon("kadu_icons/clear-history"), tr("Clear History"), false,
		disableNonHistoryContacts
	);

}

void History::deleteActionDescriptions()
{
	BuddiesListViewMenuManager::instance()->removeActionDescription(ShowHistoryActionDescription);
	Core::instance()->kaduWindow()->removeMenuActionDescription(ShowHistoryActionDescription);

	delete ShowHistoryActionDescription;
	ShowHistoryActionDescription = 0;
}

void History::showHistoryActionActivated(QAction *sender, bool toggled)
{
	Q_UNUSED(toggled)

	Action *action = qobject_cast<Action *>(sender);
	if (!action)
		return;

	ChatEditBox *chatEditBox = qobject_cast<ChatEditBox *>(sender->parent());
	Chat chat = action->chat();
	if (!chatEditBox || chat != chatEditBox->chat())
	{
		HistoryWindow::show(chat);
		return;
	}

	ChatWidget *chatWidget = chatEditBox->chatWidget();
	if (chatWidget)
	{
		QList<QWidget *> widgets = sender->associatedWidgets();
		if (widgets.size() == 0)
			return;

		QWidget *widget = widgets.at(widgets.size() - 1);

		QScopedPointer<QMenu> menu(new QMenu(chatWidget));

		if (config_file.readBoolEntry("Chat", "ChatPrune", false))
		{
			int prune = config_file.readNumEntry("Chat", "ChatPruneLen", 20);
			menu->addAction(tr("Show last %1 messages").arg(prune))->setData(0);
			menu->addSeparator();
		}

		menu->addAction(tr("Show messages since yesterday"))->setData(1);
		menu->addAction(tr("Show messages from last 7 days"))->setData(7);
		menu->addAction(tr("Show messages from last 30 days"))->setData(30);
		menu->addAction(tr("Show whole history"))->setData(-1);

		connect(menu.data(), SIGNAL(triggered(QAction *)), this, SLOT(showMoreMessages(QAction *)));

		menu->exec(widget->mapToGlobal(QPoint(0, widget->height())));
	}
}

void History::showMoreMessages(QAction *action)
{
	if (!CurrentStorage)
		return;

	ChatWidget *chatWidget = qobject_cast<ChatWidget *>(sender()->parent());
	if (!chatWidget)
		return;

	bool ok;
	int days = action->data().toInt(&ok);

	if (!ok)
		return;

	ChatMessagesView *chatMessagesView = chatWidget->chatMessagesView();
	if (!chatMessagesView)
		return;

	Chat chat = AggregateChatManager::instance()->aggregateChat(chatWidget->chat());

	chatMessagesView->setForcePruneDisabled(0 != days);
	QList<Message> messages;

	if (-1 == days)
	{
		HistoryWindow::show(chatWidget->chat());
		return;
	}
	else if (0 != days)
	{
		QDate since = QDate::currentDate().addDays(-days);
		messages = CurrentStorage->messagesSince(chat ? chat : chatWidget->chat(), since);
	}
	else
	{
		QDateTime backTo = QDateTime::currentDateTime().addDays(ChatHistoryQuotationTime/24);
		messages = CurrentStorage->messagesBackTo(chat ? chat : chatWidget->chat(), backTo, config_file.readNumEntry("Chat", "ChatPruneLen", 20));
	}

	chatMessagesView->clearMessages();
	chatMessagesView->appendMessages(messages);
}

void History::clearHistoryActionActivated(QAction *sender, bool toggled)
{
	Q_UNUSED(toggled)

	if (!CurrentStorage)
		return;

	Action *action = qobject_cast<Action *>(sender);
	if (!action)
		return;

	if (action->chat())
		CurrentStorage->clearChatHistory(action->chat());
}

void History::chatCreated(ChatWidget *chatWidget)
{

	kdebugf();

	if (!chatWidget)
		return;

	if (!CurrentStorage)
		return;

	// don't do it for already opened chats with discussions
	if (chatWidget->countMessages())
		return;

	ChatMessagesView *chatMessagesView = chatWidget->chatMessagesView();
	if (!chatMessagesView)
		return;

	QList<Message> messages;

	unsigned int chatHistoryQuotation = qMax(ChatHistoryCitation, PendingMessagesManager::instance()->pendingMessagesForChat(chatWidget->chat()).size());

	Chat chat = AggregateChatManager::instance()->aggregateChat(chatWidget->chat());

	QDateTime backTo = QDateTime::currentDateTime().addSecs(ChatHistoryQuotationTime * 3600);
	messages = CurrentStorage->messagesBackTo(chat ? chat : chatWidget->chat(), backTo, chatHistoryQuotation);

	if (messages.isEmpty())
		return;

	chatMessagesView->appendMessages(messages);

	kdebugf2();
}

void History::accountRegistered(Account account)
{
	if (!account.protocolHandler())
		return;

	connect(account, SIGNAL(buddyStatusChanged(Contact, Status)),
			this, SLOT(contactStatusChanged(Contact, Status)));

	ChatService *service = account.protocolHandler()->chatService();
	if (service)
	{
		connect(service, SIGNAL(messageReceived(const Message &)),
				this, SLOT(enqueueMessage(const Message &)));
		connect(service, SIGNAL(messageSent(const Message &)),
				this, SLOT(enqueueMessage(const Message &)));
	}
}

void History::accountUnregistered(Account account)
{
	if (!account.protocolHandler())
		return;

	disconnect(account, SIGNAL(buddyStatusChanged(Contact, Status)),
			this, SLOT(contactStatusChanged(Contact, Status)));

	ChatService *service = account.protocolHandler()->chatService();
	if (service)
	{
		disconnect(service, SIGNAL(messageReceived(const Message &)),
				this, SLOT(enqueueMessage(const Message &)));
		disconnect(service, SIGNAL(messageSent(const Message &)),
				this, SLOT(enqueueMessage(const Message &)));
	}
}

void History::enqueueMessage(const Message &message)
{
	if (!CurrentStorage || !SaveChats)
		return;

	if (!SaveChatsWithAnonymous && message.messageChat().contacts().count() == 1
		&& (*message.messageChat().contacts().constBegin()).ownerBuddy().isAnonymous())
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
		return qMakePair(Contact::null, Status("Offline"));

	return UnsavedStatusChanges.dequeue();
}

void History::crash()
{
	// don't try to stop/wait on thread here, see http://kadu.net/mantis/view.php?id=2228
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
		SaveThread->wait(10000);

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
	dontCiteOldMessagesLabel = static_cast<QLabel *>(mainConfigurationWindow->widget()->widgetById("history/dontCiteOldMessagesLabel"));
	connect(mainConfigurationWindow->widget()->widgetById("history/dontCiteOldMessages"), SIGNAL(valueChanged(int)),
		this, SLOT(updateQuoteTimeLabel(int)));

	connect(mainConfigurationWindow->widget()->widgetById("history/save"), SIGNAL(toggled(bool)),
		mainConfigurationWindow->widget()->widgetById("history/savechatswithanonymous"), SLOT(setEnabled(bool)));

	connect(mainConfigurationWindow->widget()->widgetById("history/savestatuschanges"), SIGNAL(toggled(bool)),
		mainConfigurationWindow->widget()->widgetById("history/saveonlystatuswithdescription"), SLOT(setEnabled(bool)));
}

void History::updateQuoteTimeLabel(int value)
{
	dontCiteOldMessagesLabel->setText(tr("%1 day(s) %2 hour(s)").arg(-value / 24).arg((-value) % 24));
}

void History::configurationUpdated()
{
	kdebugf();

	ChatHistoryCitation = config_file.readNumEntry("History", "ChatHistoryCitation");
	ChatHistoryQuotationTime = config_file.readNumEntry("History", "ChatHistoryQuotationTime", -744);

	SaveChats = config_file.readBoolEntry("History", "SaveChats", true);
	SaveChatsWithAnonymous = config_file.readBoolEntry("History", "SaveChatsWithAnonymous", true);
	SaveStatuses = config_file.readBoolEntry("History", "SaveStatusChanges", false);
	SaveOnlyStatusesWithDescription = config_file.readBoolEntry("History", "SaveOnlyStatusWithDescription", false);

	kdebugf2();
}

bool History::removeContactFromStorage(Buddy buddy)
{
	if (!CurrentStorage)
		return true;

	// TODO: optimize
	foreach (const Chat &chat, ChatManager::instance()->items())
		if (chat.contacts().toBuddySet().contains(buddy) && !CurrentStorage->chatDates(chat, HistorySearchParameters()).isEmpty())
			return false;

	return true;
}

void History::registerStorage(HistoryStorage *storage)
{
	CurrentStorage = storage;

	stopSaveThread();

	if (!CurrentStorage)
		return;

	startSaveThread();

	foreach (ChatWidget *chat, ChatWidgetManager::instance()->chats())
		chatCreated(chat);

	foreach (const Account &account, AccountManager::instance()->items())
		accountRegistered(account);
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
}

QList<Chat> History::chatsList(const HistorySearchParameters &search)
{
	kdebugf();

	return CurrentStorage->chats(search);
}

QList<DatesModelItem> History::datesForChat(const Chat &chat, const HistorySearchParameters &search)
{
	kdebugf();

	return CurrentStorage->chatDates(chat, search);
}

QList<Message> History::messages(const Chat &chat, const QDate &date, int limit)
{
	kdebugf();

	return CurrentStorage->messages(chat, date, limit);
}

QList<Buddy> History::statusBuddiesList(const HistorySearchParameters &search)
{
	kdebugf();

	return CurrentStorage->statusBuddiesList(search);
}

QList<DatesModelItem> History::datesForStatusBuddy(const Buddy &buddy, const HistorySearchParameters &search)
{
	kdebugf();

	return CurrentStorage->datesForStatusBuddy(buddy, search);
}

QList<TimedStatus> History::statuses(const Buddy &buddy, const QDate &date, int limit)
{
	kdebugf();

	return CurrentStorage->statuses(buddy, date, limit);
}

QList<QString> History::smsRecipientsList(const HistorySearchParameters &search)
{
	kdebugf();

	return CurrentStorage->smsRecipientsList(search);
}

QList<DatesModelItem> History::datesForSmsRecipient(const QString &recipient, const HistorySearchParameters &search)
{
	kdebugf();

	return CurrentStorage->datesForSmsRecipient(recipient, search);
}

QList<Message> History::sms(const QString &recipient, const QDate &date, int limit)
{
	kdebugf();

	return CurrentStorage->sms(recipient, date, limit);
}

void History::deleteHistory(const Buddy &buddy)
{
	if (CurrentStorage)
		CurrentStorage->deleteHistory(buddy);
}

void History::createDefaultConfiguration()
{
	config_file.addVariable("History", "SaveChats", config_file.readBoolEntry("History", "Logging", true));
	config_file.removeVariable("History", "Logging");

	config_file.addVariable("ShortCuts", "kadu_viewhistory", "Ctrl+H");

	config_file.addVariable("History", "SaveStatusChanges", true);

	config_file.addVariable("History", "SaveChatsWithAnonymous", true);

	config_file.addVariable("History", "SaveOnlyStatusWithDescription", true);

	config_file.addVariable("History", "ChatHistoryCitation", 10);
	config_file.addVariable("History", "ChatHistoryQuotationTime", -24);
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
