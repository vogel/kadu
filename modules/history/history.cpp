/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <QtCore/QList>
#include <QtGui/QKeyEvent>
#include <QtGui/QLabel>
#include <QtGui/QGridLayout>
#include <QtGui/QPushButton>

#include "accounts/account.h"
#include "accounts/account-manager.h"
#include "contacts/contact-manager.h"
#include "chat/chat.h"
#include "contacts/contact.h"
#include "core/core.h"
#include "gui/widgets/configuration/config-group-box.h"
#include "gui/widgets/configuration/configuration-widget.h"
#include "misc/path-conversion.h"
#include "protocols/services/chat-service.h"

#include "debug.h"
#include "icons_manager.h"
#include "message_box.h"

#include "history.h"

extern "C" KADU_EXPORT int history_init(bool firstLoad)
{
	kdebugf();
	History *h = History::instance();
	MainConfigurationWindow::registerUiFile(dataPath("kadu/modules/configuration/history.ui"));
	kdebugf2();
	return 0;
}

extern "C" KADU_EXPORT void history_close()
{
	kdebugf();
	MainConfigurationWindow::unregisterUiFile(dataPath("kadu/modules/configuration/history.ui"));
	kdebugf2();
}

void disableNonHistoryContacts(KaduAction *action)
{
	kdebugf();
	action->setEnabled(false);
	ContactSet contacts = action->contacts();

	if (!contacts.count())
		return;

	foreach (const Contact &contact, contacts)
	{
		if (Core::instance()->myself() == contact)
			return;

		Account *account = contact.prefferedAccount();
		if (!account || !account->protocol()->chatService())
			return;
	}

	action->setEnabled(true);
	kdebugf2();
}

History * History::Instance = 0;

History * History::instance()
{
	if (!Instance)
		Instance = new History();
	return Instance;
}

History::History() : QObject(NULL), HistoryDialog(new HistoryDlg()), CurrentStorage(0)
{
	kdebugf();
	createActionDescriptions();
	connect(AccountManager::instance(), SIGNAL(accountRegistered(Account *)),
		this, SLOT(accountRegistered(Account *)));
	connect(AccountManager::instance(), SIGNAL(accountUnregistered(Account *)),
		this, SLOT(accountUnregistered(Account *)));
	kdebugf2();
}

History::~History()
{
	kdebugf();
	deleteActionDescriptions();
	kdebugf2();
}

void History::createActionDescriptions()
{
	ShowHistoryActionDescription = new ActionDescription(0,
		ActionDescription::TypeUser, "showHistoryAction",
		this, SLOT(showHistoryActionActivated(QAction *, bool)),
		"History", tr("Show history"), false, QString::null,
		disableNonHistoryContacts
	);
	ShowHistoryActionDescription->setShortcut("kadu_showhistory");
	ContactsListWidgetMenuManager::instance()->insertActionDescription(1, ShowHistoryActionDescription);
}

void History::deleteActionDescriptions()
{
	ContactsListWidgetMenuManager::instance()->removeActionDescription(ShowHistoryActionDescription);

	delete ShowHistoryActionDescription;
	ShowHistoryActionDescription = 0;
}

void History::showHistoryActionActivated(QAction *sender, bool toggled)
{
	kdebugf();
	KaduMainWindow *window = dynamic_cast<KaduMainWindow *>(sender->parent());
	if (window)
		HistoryDialog->show(window->contacts());
	kdebugf2();
}

void History::accountRegistered(Account *account)
{
	ChatService *service = account->protocol()->chatService();
	if (service)
	{
		connect(service, SIGNAL(messageReceived(Chat *, Contact , const QString &)),
			CurrentStorage, SLOT(messageReceived(Chat *, Contact , const QString &)));
		connect(service, SIGNAL(messageSent(Chat *, const QString &)),
			CurrentStorage, SLOT(messageSent(Chat *, const QString &)));
	}
}


void History::accountUnregistered(Account *account)
{
	ChatService *service = account->protocol()->chatService();
	if (service)
	{
		disconnect(service, SIGNAL(messageReceived(Chat *, Contact , const QString &)),
			CurrentStorage, SLOT(messageReceived(Chat *, Contact , const QString &)));
		disconnect(service, SIGNAL(messageSent(Chat *, const QString &)),
			CurrentStorage, SLOT(messageSent(Chat *, const QString &)));
	}
}

void History::mainConfigurationWindowCreated(MainConfigurationWindow *mainConfigurationWindow)
{

	ConfigGroupBox *chatsGroupBox = mainConfigurationWindow->widget()->configGroupBox("Chat", "History", "Chats history");
	QWidget *selectedChatsUsersWidget = new QWidget(chatsGroupBox->widget());
	QGridLayout *selectedChatsUsersLayout = new QGridLayout(selectedChatsUsersWidget);
	selectedChatsUsersLayout->setSpacing(5);
	selectedChatsUsersLayout->setMargin(5);

	allChatsUsers = new QListWidget(selectedChatsUsersWidget);
	QPushButton *moveToSelectedChatsList = new QPushButton(tr("Move to 'Selected list'"), selectedChatsUsersWidget);

	selectedChatsUsersLayout->addWidget(new QLabel(tr("User list"), selectedChatsUsersWidget), 0, 0);
	selectedChatsUsersLayout->addWidget(allChatsUsers, 1, 0);
	selectedChatsUsersLayout->addWidget(moveToSelectedChatsList, 2, 0);

	selectedChatsUsers = new QListWidget(selectedChatsUsersWidget);
	QPushButton *moveToAllChatsList = new QPushButton(tr("Move to 'User list'"), selectedChatsUsersWidget);

	selectedChatsUsersLayout->addWidget(new QLabel(tr("Selected list"), selectedChatsUsersWidget), 0, 1);
	selectedChatsUsersLayout->addWidget(selectedChatsUsers, 1, 1);
	selectedChatsUsersLayout->addWidget(moveToAllChatsList, 2, 1);

	connect(moveToSelectedChatsList, SIGNAL(clicked()), this, SLOT(moveToSelectedChatsList()));
	connect(moveToAllChatsList, SIGNAL(clicked()), this, SLOT(moveToAllChatsList()));

	chatsGroupBox->addWidgets(0, selectedChatsUsersWidget);
/*	foreach(const UserListElement &user, *userlist)
		if (!user.protocolList().isEmpty() && !user.isAnonymous())
			if (!user.data("history_save_chats").toBool())
				allChatsUsers->addItem(user.altNick());
			else
				selectedChatsUsers->addItem(user.altNick());*/

	allChatsUsers->sortItems();
	selectedChatsUsers->sortItems();
	allChatsUsers->setSelectionMode(QAbstractItemView::ExtendedSelection);
	selectedChatsUsers->setSelectionMode(QAbstractItemView::ExtendedSelection);

	connect(selectedChatsUsers, SIGNAL(doubleClicked(QListWidgetItem *)), this, SLOT(moveToAllChatsList()));
	connect(allChatsUsers, SIGNAL(doubleClicked(QListWidgetItem *)), this, SLOT(moveToSelectedChatsList()));

	ConfigGroupBox *statusGroupBox = mainConfigurationWindow->widget()->configGroupBox("Chat", "History", "Status changes");
	QWidget *selectedStatusUsersWidget = new QWidget(statusGroupBox->widget());
	QGridLayout *selectedStatusUsersLayout = new QGridLayout(selectedStatusUsersWidget);
	selectedStatusUsersLayout->setSpacing(5);
	selectedStatusUsersLayout->setMargin(5);

	allStatusUsers = new QListWidget(selectedStatusUsersWidget);
	QPushButton *moveToSelectedStatusList = new QPushButton(tr("Move to 'Selected list'"), selectedStatusUsersWidget);

	selectedStatusUsersLayout->addWidget(new QLabel(tr("User list"), selectedStatusUsersWidget), 0, 0);
	selectedStatusUsersLayout->addWidget(allStatusUsers, 1, 0);
	selectedStatusUsersLayout->addWidget(moveToSelectedStatusList, 2, 0);

	selectedStatusUsers = new QListWidget(selectedStatusUsersWidget);
	QPushButton *moveToAllStatusList = new QPushButton(tr("Move to 'User list'"), selectedStatusUsersWidget);

	selectedStatusUsersLayout->addWidget(new QLabel(tr("Selected list"), selectedStatusUsersWidget), 0, 1);
	selectedStatusUsersLayout->addWidget(selectedStatusUsers, 1, 1);
	selectedStatusUsersLayout->addWidget(moveToAllStatusList, 2, 1);

	connect(moveToSelectedStatusList, SIGNAL(clicked()), this, SLOT(moveToSelectedStatusList()));
	connect(moveToAllStatusList, SIGNAL(clicked()), this, SLOT(moveToAllStatusList()));

	statusGroupBox->addWidgets(0, selectedStatusUsersWidget);
/*	foreach(const UserListElement &user, *userlist)
		if (!user.protocolList().isEmpty() && !user.isAnonymous())
			if (!user.data("history_save_status").toBool())
				allStatusUsers->addItem(user.altNick());
			else
				selectedStatusUsers->addItem(user.altNick());
*/
	allStatusUsers->sortItems();
	selectedStatusUsers->sortItems();
	allStatusUsers->setSelectionMode(QAbstractItemView::ExtendedSelection);
	selectedStatusUsers->setSelectionMode(QAbstractItemView::ExtendedSelection);

	connect(selectedStatusUsers, SIGNAL(doubleClicked(QListWidgetItem *)), this, SLOT(moveToAllStatusList()));
	connect(allStatusUsers, SIGNAL(doubleClicked(QListWidgetItem *)), this, SLOT(moveToSelectedStatusList()));
// 	dontCiteOldMessagesLabel = dynamic_cast<QLabel *>(mainConfigurationWindow->widget()->widgetById("history/dontCiteOldMessagesLabel"));
// 	connect(mainConfigurationWindow->widget()->widgetById("history/dontCiteOldMessages"), SIGNAL(valueChanged(int)),
// 		this, SLOT(updateQuoteTimeLabel(int)));

	connect(mainConfigurationWindow->widget()->widgetById("history/save"), SIGNAL(toggled(bool)),
		mainConfigurationWindow->widget()->widgetById("history/chats"), SLOT(setEnabled(bool)));

	connect(mainConfigurationWindow->widget()->widgetById("history/save"), SIGNAL(toggled(bool)),
		mainConfigurationWindow->widget()->widgetById("history/statusChanges"), SLOT(setEnabled(bool)));

	connect(mainConfigurationWindow->widget()->widgetById("history/save"), SIGNAL(toggled(bool)),
		mainConfigurationWindow->widget()->widgetById("history/citation"), SLOT(setEnabled(bool)));

	connect(mainConfigurationWindow->widget()->widgetById("history/savestatusforall"), SIGNAL(toggled(bool)), selectedStatusUsersWidget, SLOT(setDisabled(bool)));

	connect(mainConfigurationWindow->widget()->widgetById("history/savechats"), SIGNAL(toggled(bool)),
		mainConfigurationWindow->widget()->widgetById("history/savechatswithanonymous"), SLOT(setEnabled(bool)));
	connect(mainConfigurationWindow->widget()->widgetById("history/savechats"), SIGNAL(toggled(bool)),
		mainConfigurationWindow->widget()->widgetById("history/saveundeliveredmsgs"), SLOT(setEnabled(bool)));
	connect(mainConfigurationWindow->widget()->widgetById("history/savechats"), SIGNAL(toggled(bool)),
		selectedChatsUsersWidget, SLOT(setEnabled(bool)));
	connect(mainConfigurationWindow->widget()->widgetById("history/savechats"), SIGNAL(toggled(bool)),
		mainConfigurationWindow->widget()->widgetById("history/savechatsforall"), SLOT(setEnabled(bool)));

	connect(mainConfigurationWindow->widget()->widgetById("history/savechatsforall"), SIGNAL(toggled(bool)), selectedChatsUsersWidget, SLOT(setDisabled(bool)));
	connect(mainConfigurationWindow->widget()->widgetById("history/savestatuschanges"), SIGNAL(toggled(bool)),
		mainConfigurationWindow->widget()->widgetById("history/ignoresomestatuschanges"), SLOT(setEnabled(bool)));
	connect(mainConfigurationWindow->widget()->widgetById("history/savestatuschanges"), SIGNAL(toggled(bool)),
		mainConfigurationWindow->widget()->widgetById("history/saveonlystatuswithdescription"), SLOT(setEnabled(bool)));
	connect(mainConfigurationWindow->widget()->widgetById("history/savestatuschanges"), SIGNAL(toggled(bool)),
		selectedStatusUsersWidget, SLOT(setEnabled(bool)));
	connect(mainConfigurationWindow->widget()->widgetById("history/savestatuschanges"), SIGNAL(toggled(bool)),
		mainConfigurationWindow->widget()->widgetById("history/savestatusforall"), SLOT(setEnabled(bool)));
	connect(mainConfigurationWindow, SIGNAL(configurationWindowApplied()), this, SLOT(configurationWindowApplied()));
}

void History::updateQuoteTimeLabel(int value)
{
	dontCiteOldMessagesLabel->setText(tr("%1 day(s) %2 hour(s)").arg(-value / 24).arg((-value) % 24));
}

void History::configurationWindowApplied()
{
/*
	int count = selectedStatusUsers->count();
	for (int i = 0; i < count; i++)
		userlist->byAltNick(selectedStatusUsers->item(i)->text()).setData("history_save_status", true, true, i+1 == count);

	count = allStatusUsers->count();
	for (int i = 0; i < count; i++)
		userlist->byAltNick(allStatusUsers->item(i)->text()).setData("history_save_status", false, true, i+1 == count);

  	count = selectedChatsUsers->count();
	for (int i = 0; i < count; i++)
		userlist->byAltNick(selectedChatsUsers->item(i)->text()).setData("history_save_chats", true, true, i+1 == count );

	count = allChatsUsers->count();
	for (int i = 0; i < count; i++)
		userlist->byAltNick(allChatsUsers->item(i)->text()).setData("history_save_chats", false, true, i+1 == count);
*/
}

void History::configurationUpdated()
{
	kdebugf();
	//?
	kdebugf2();
}

void History::registerStorage(HistoryStorage *storage)
{
	CurrentStorage = storage;

	foreach (Account *account, AccountManager::instance()->accounts())
		accountRegistered(account);
}

void History::unregisterStorage(HistoryStorage *storage)
{
	if (CurrentStorage != storage)
		return;

	foreach (Account *account, AccountManager::instance()->accounts())
		accountUnregistered(account);

	delete CurrentStorage;
	CurrentStorage = 0;
}

QList<Chat *> History::chatsList()
{
	return CurrentStorage->chatsList();
}

QList<QDate> History::datesForChat(Chat *chat)
{
	return CurrentStorage->datesForChat(chat);
}

QList<ChatMessage *> History::getMessages(Chat *chat, QDate date, int limit)
{
	return CurrentStorage->getMessages(chat, date, limit);
}

int History::getMessagesCount(Chat *chat, QDate date)
{
	return CurrentStorage->getMessagesCount(chat, date);
}

