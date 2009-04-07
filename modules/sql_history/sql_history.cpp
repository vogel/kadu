/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <QtCore/QList>
#include <QtCore/QRegExp>
#include <QtGui/QKeyEvent>
#include <QtGui/QLabel>
#include <QtGui/QGridLayout>
#include <QtNetwork/QHostAddress>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>

#include "accounts/account.h"
#include "accounts/account_manager.h"
#include "action.h"
#include "chat_manager-old.h"
#include "debug.h"
#include "icons_manager.h"
#include "message_box.h"
#include "misc/misc.h"
#include "kadu.h"
#include "userlistelement.h"

#include "sql_history.h"

extern "C" KADU_EXPORT int sql_history_init(bool firstLoad)
{
	kdebugf();
	sql_history = new SqlHistory(firstLoad);
	MainConfigurationWindow::registerUiFile(dataPath("kadu/modules/configuration/sql_history.ui"), sql_history);
	kdebugf2();
	return 0;
}

extern "C" KADU_EXPORT void sql_history_close()
{
	kdebugf();
	MainConfigurationWindow::unregisterUiFile(dataPath("kadu/modules/configuration/sql_history.ui"), sql_history);
	QSqlDatabase::removeDatabase("kadu-history");
	kdebugf2();
}

void SqlHistory::mainConfigurationWindowCreated(MainConfigurationWindow *mainConfigurationWindow)
{
	ConfigGroupBox *chatsGroupBox = mainConfigurationWindow->configGroupBox("Chat", "History", "Chats");
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
	foreach(const UserListElement &user, *userlist)
		if (!user.protocolList().isEmpty() && !user.isAnonymous())
			if (!user.data("history_save_chats").toBool())
				allChatsUsers->addItem(user.altNick());
			else
				selectedChatsUsers->addItem(user.altNick());

	allChatsUsers->sortItems();
	selectedChatsUsers->sortItems();
	allChatsUsers->setSelectionMode(QAbstractItemView::ExtendedSelection);
	selectedChatsUsers->setSelectionMode(QAbstractItemView::ExtendedSelection);

	connect(selectedChatsUsers, SIGNAL(doubleClicked(QListWidgetItem *)), this, SLOT(moveToAllChatsList()));
	connect(allChatsUsers, SIGNAL(doubleClicked(QListWidgetItem *)), this, SLOT(moveToSelectedChatsList()));

	ConfigGroupBox *statusGroupBox = mainConfigurationWindow->configGroupBox("Chat", "History", "Status changes");
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
	foreach(const UserListElement &user, *userlist)
		if (!user.protocolList().isEmpty() && !user.isAnonymous())
			if (!user.data("history_save_status").toBool())
				allStatusUsers->addItem(user.altNick());
			else
				selectedStatusUsers->addItem(user.altNick());

	allStatusUsers->sortItems();
	selectedStatusUsers->sortItems();
	allStatusUsers->setSelectionMode(QAbstractItemView::ExtendedSelection);
	selectedStatusUsers->setSelectionMode(QAbstractItemView::ExtendedSelection);

	connect(selectedStatusUsers, SIGNAL(doubleClicked(QListWidgetItem *)), this, SLOT(moveToAllStatusList()));
	connect(allStatusUsers, SIGNAL(doubleClicked(QListWidgetItem *)), this, SLOT(moveToSelectedStatusList()));
	dontCiteOldMessagesLbl = dynamic_cast<QLabel *>(mainConfigurationWindow->widgetById("sql_history/dontCiteOldMessagesLbl"));
	connect(mainConfigurationWindow->widgetById("sql_history/dontCiteOldMessages"), SIGNAL(valueChanged(int)),
		this, SLOT(updateQuoteTimeLabel(int)));

	connect(mainConfigurationWindow->widgetById("sql_history/save"), SIGNAL(toggled(bool)),
		mainConfigurationWindow->widgetById("sql_history/chats"), SLOT(setEnabled(bool)));

	connect(mainConfigurationWindow->widgetById("sql_history/save"), SIGNAL(toggled(bool)),
		mainConfigurationWindow->widgetById("sql_history/statusChanges"), SLOT(setEnabled(bool)));

	connect(mainConfigurationWindow->widgetById("sql_history/save"), SIGNAL(toggled(bool)),
		mainConfigurationWindow->widgetById("sql_history/citation"), SLOT(setEnabled(bool)));

	connect(mainConfigurationWindow->widgetById("sql_history/savestatusforall"), SIGNAL(toggled(bool)), selectedStatusUsersWidget, SLOT(setDisabled(bool)));

	connect(mainConfigurationWindow->widgetById("sql_history/savechats"), SIGNAL(toggled(bool)),
		mainConfigurationWindow->widgetById("sql_history/savechatswithanonymous"), SLOT(setEnabled(bool)));
	connect(mainConfigurationWindow->widgetById("sql_history/savechats"), SIGNAL(toggled(bool)),
		mainConfigurationWindow->widgetById("sql_history/saveundeliveredmsgs"), SLOT(setEnabled(bool)));
	connect(mainConfigurationWindow->widgetById("sql_history/savechats"), SIGNAL(toggled(bool)),
		selectedChatsUsersWidget, SLOT(setEnabled(bool)));
	connect(mainConfigurationWindow->widgetById("sql_history/savechats"), SIGNAL(toggled(bool)),
		mainConfigurationWindow->widgetById("sql_history/savechatsforall"), SLOT(setEnabled(bool)));

	connect(mainConfigurationWindow->widgetById("sql_history/savechatsforall"), SIGNAL(toggled(bool)), selectedChatsUsersWidget, SLOT(setDisabled(bool)));
	connect(mainConfigurationWindow->widgetById("sql_history/savestatuschanges"), SIGNAL(toggled(bool)),
		mainConfigurationWindow->widgetById("sql_history/ignoresomestatuschanges"), SLOT(setEnabled(bool)));
	connect(mainConfigurationWindow->widgetById("sql_history/savestatuschanges"), SIGNAL(toggled(bool)),
		mainConfigurationWindow->widgetById("sql_history/saveonlystatuswithdescription"), SLOT(setEnabled(bool)));
	connect(mainConfigurationWindow->widgetById("sql_history/savestatuschanges"), SIGNAL(toggled(bool)),
		selectedStatusUsersWidget, SLOT(setEnabled(bool)));
	connect(mainConfigurationWindow->widgetById("sql_history/savestatuschanges"), SIGNAL(toggled(bool)),
		mainConfigurationWindow->widgetById("sql_history/savestatusforall"), SLOT(setEnabled(bool)));
	connect(mainConfigurationWindow, SIGNAL(configurationWindowApplied()), this, SLOT(configurationWindowApplied()));

	connect(mainConfigurationWindow->widgetById("sql_history/historyAdvanced"), SIGNAL(clicked()), this, SLOT(showHistoryAdvanced()));
	
}

void SqlHistory::showHistoryAdvanced()
{
	kdebugf();
	if (!historyAdvanced)
	{
		historyAdvanced = new ConfigurationWindow("history-advanced", tr("Advenced history's configuration"));
		historyAdvanced->appendUiFile(dataPath("kadu/modules/configuration/history-advanced.ui"));

		portSpinBox = dynamic_cast<QSpinBox *>(historyAdvanced->widgetById("sql_history/databasehostport"));
		connect(portSpinBox, SIGNAL(valueChanged(int)), this, SLOT(portSpinBoxValueChanged(int)));
	
		driverComboBox = dynamic_cast<QComboBox *>(historyAdvanced->widgetById("sql_history/dbdriver"));
		connect(driverComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(driverComboBoxValueChanged(int)));

		hostLineEdit = dynamic_cast<QLineEdit *>(historyAdvanced->widgetById("sql_history/databasehost"));
		nameLineEdit = dynamic_cast<QLineEdit *>(historyAdvanced->widgetById("sql_history/databasename"));
		userLineEdit = dynamic_cast<QLineEdit *>(historyAdvanced->widgetById("sql_history/databaseuser"));
		passLineEdit = dynamic_cast<QLineEdit *>(historyAdvanced->widgetById("sql_history/databasepass"));
		prefixLineEdit = dynamic_cast<QLineEdit *>(historyAdvanced->widgetById("sql_history/databaseprefix"));

		if(config_file.readEntry("History", "DatabaseDriver") == "QSQLITE")
		{
			portSpinBox->setEnabled(false);
			hostLineEdit->setEnabled(false); 
			userLineEdit->setEnabled(false); 
			nameLineEdit->setEnabled(false); 
			passLineEdit->setEnabled(false);
			prefixLineEdit->setEnabled(false);
		}
		connect(historyAdvanced, SIGNAL(destroyed()), this, SLOT(historyAdvancedDestroyed()));
	}

	historyAdvanced->show();
	kdebugf2();
}

void SqlHistory::historyAdvancedDestroyed()
{
	historyAdvanced = 0;
}

void SqlHistory::configurationWindowApplied()
{
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
}

void SqlHistory::driverComboBoxValueChanged(int index)
{

	if(driverComboBox->itemText(index) == "SQLite")
	{
		portSpinBox->setEnabled(false);
		hostLineEdit->setEnabled(false); 
		userLineEdit->setEnabled(false); 
		nameLineEdit->setEnabled(false); 
		passLineEdit->setEnabled(false);
		prefixLineEdit->setEnabled(false);
	}
	else if(driverComboBox->itemText(index) == "MySQL")
	{
		portSpinBox->setEnabled(true);
		portSpinBox->setValue(3306);
		hostLineEdit->setEnabled(true);
		userLineEdit->setEnabled(true); 
		nameLineEdit->setEnabled(true); 
		passLineEdit->setEnabled(true);
		prefixLineEdit->setEnabled(true);
	}
	else if(driverComboBox->itemText(index) == "PostrgeSQL")
	{
		portSpinBox->setEnabled(true);
		portSpinBox->setValue(5432);
		hostLineEdit->setEnabled(true);
		userLineEdit->setEnabled(true); 
		nameLineEdit->setEnabled(true); 
		passLineEdit->setEnabled(true);
		prefixLineEdit->setEnabled(true);
	}

}

void SqlHistory::updateQuoteTimeLabel(int value)
{
	dontCiteOldMessagesLbl->setText(tr("%1 day(s) %2 hour(s)").arg(-value / 24).arg((-value) % 24));
}

void SqlHistory::portSpinBoxValueChanged(int value)
{
	if (portSpinBox->value() < value)
		portSpinBox->setValue(value);
}

void SqlHistory::moveToAllStatusList()
{
	int count = selectedStatusUsers->count();
 
	for (int i = count - 1; i >= 0; i--)
		if (selectedStatusUsers->item(i)->isSelected())
		{
			allStatusUsers->addItem(selectedStatusUsers->item(i)->text());
			QListWidgetItem *item = selectedStatusUsers->takeItem(i);
			delete item;
		}
 
	allStatusUsers->sortItems();
}

void SqlHistory::moveToSelectedStatusList()
{
	int count = allStatusUsers->count();
	for (int i = count - 1; i >= 0; i--)
		if (allStatusUsers->item(i)->isSelected())
		{
			selectedStatusUsers->addItem(allStatusUsers->item(i)->text());
			QListWidgetItem *item = allStatusUsers->takeItem(i);
			delete item;
		}
	selectedStatusUsers->sortItems();
}

void SqlHistory::moveToAllChatsList()
{
	int count = selectedChatsUsers->count();
 
	for (int i = count - 1; i >= 0; i--)
		if (selectedChatsUsers->item(i)->isSelected())
		{
			allChatsUsers->addItem(selectedChatsUsers->item(i)->text());
			QListWidgetItem *item = selectedChatsUsers->takeItem(i);
			delete item;
		}
 
	allChatsUsers->sortItems();
}

void SqlHistory::moveToSelectedChatsList()
{
	int count = allChatsUsers->count();
 
	for (int i = count - 1; i >= 0; i--)
		if (allChatsUsers->item(i)->isSelected())
		{
			selectedChatsUsers->addItem(allChatsUsers->item(i)->text());
			QListWidgetItem *item = allChatsUsers->takeItem(i);
			delete item;
		}
 
	selectedChatsUsers->sortItems();
}

SqlHistory::SqlHistory(bool firstLoad) : QObject(NULL, "history"), historyAdvanced(0)
{
	kdebugf();
	createDefaultConfiguration();

	connect(chat_manager, SIGNAL(messageSentAndConfirmed(UserListElements, const QString&)),
		this, SLOT(messageSentAndConfirmed(UserListElements, const QString&)));
	connect(chat_manager, SIGNAL(chatWidgetCreated(ChatWidget *, time_t)),
		this, SLOT(chatCreated(ChatWidget*)));
	connect(chat_manager, SIGNAL(chatWidgetDestroying(ChatWidget *)), this, SLOT(chatDestroying(ChatWidget *)));
	connect(kadu, SIGNAL(removingUsers(UserListElements)),
		this, SLOT(removingUsers(UserListElements)));
	connect(userlist, SIGNAL(statusChanged(UserListElement, QString, const UserStatus &, bool, bool)),
		this, SLOT(statusChanged(UserListElement, QString, const UserStatus &, bool, bool)));

	connect(AccountManager::instance(), SIGNAL(accountRegistered(Account *)),
		this, SLOT(accountRegistered(Account *)));
	connect(AccountManager::instance(), SIGNAL(accountUnregistered(Account *)),
		this, SLOT(accountUnregistered(Account *)));

	foreach (Account *account, AccountManager::instance()->accounts())
		accountRegistered(account);

	foreach(ChatWidget *it, chat_manager->chats())
		chatCreated(it);

	showHistoryActionDescription = new ActionDescription(
		ActionDescription::TypeUser, "showHistoryAction",
		this, SLOT(historyActionActivated(QAction *, bool)),
		"History", tr("View history"), false, "", 0
	);
	UserBox::insertActionDescription(5, showHistoryActionDescription); 
	if (firstLoad)
	{
		Kadu::addAction("showHistoryAction");
		ChatEditBox::addAction("showHistoryAction");
	}

	clearHistoryActionDescription = new ActionDescription(
		ActionDescription::TypeUser, "clearHistoryAction",
		this, SLOT(clearHistoryActionActivated(QAction *, bool)),
		"ClearHistory", tr("Clear history"), false, "", 0
	);
	UserBox::insertManagementActionDescription(7, clearHistoryActionDescription);
	//menu z dialoga historii... mo¿e mo¿na lepiej, ale nie mog³o byæ w dialogu, gdy¿ dodatkowe modu³y go wymagaj±, a dialog nie zawsze istnieje
	historyDialog = new HistoryDlg;
	initializeDatabase();
	kdebugf2();
}

SqlHistory::~SqlHistory()
{
	kdebugf();
	UserBox::removeActionDescription(showHistoryActionDescription);
	delete showHistoryActionDescription;
	showHistoryActionDescription = 0;
	UserBox::removeManagementActionDescription(clearHistoryActionDescription);
	delete clearHistoryActionDescription;
	clearHistoryActionDescription = 0;

	foreach (Account *account, AccountManager::instance()->accounts())
		accountUnregistered(account);

	disconnect(kadu, SIGNAL(removingUsers(UserListElements)), this, SLOT(removingUsers(UserListElements)));
	disconnect(chat_manager, SIGNAL(messageSentAndConfirmed(UserListElements,
						const QString&)),
		this, SLOT(messageSentAndConfirmed(UserListElements, const QString&)));
	disconnect(userlist, SIGNAL(statusChanged(UserListElement, QString, const UserStatus &, bool, bool)), this, SLOT(statusChanged(UserListElement, QString, const UserStatus &, bool, bool)));
	disconnect(chat_manager, SIGNAL(chatWidgetCreated(ChatWidget *)), this, SLOT(chatCreated(ChatWidget *)));
	disconnect(chat_manager, SIGNAL(chatWidgetDestroying(ChatWidget *)), this, SLOT(chatDestroying(ChatWidget *)));
 
	foreach(ChatWidget* it, chat_manager->chats())
		chatDestroying(it);
	delete historyDialog;
	historyDialog = 0;
	kdebugf2();
}

void SqlHistory::accountRegistered(Account *account)
{
	Protocol *protocol = account->protocol();

	connect(protocol, SIGNAL(messageReceived(Protocol *, UserListElements, const QString&, time_t)),
		this, SLOT(chatMsgReceived(Protocol *, UserListElements, const QString &, time_t)));
}


void SqlHistory::accountUnregistered(Account *account)
{
	Protocol *protocol = account->protocol();

	disconnect(protocol, SIGNAL(messageReceived(Protocol *, UserListElements, const QString &, time_t, bool &)),
		this, SLOT(chatMsgReceived(Protocol *, UserListElements, const QString &, time_t, bool &)));
}

void SqlHistory::clearHistoryActionActivated(QAction *sender, bool toggled)
{
	kdebugf();
	
	KaduMainWindow *window = dynamic_cast<KaduMainWindow *>(sender->parent());
	if (!window)
		return;
	sql_history->deleteHistory(window->userListElements());

	kdebugf2();
}


void SqlHistory::statusChanged(UserListElement elem, QString protocolName, const UserStatus & oldStatus, bool /*massively*/, bool /*last*/)
{
	kdebugf();
	//³ohoho...
	if (!config_file.readBoolEntry("History", "Enable") || !config_file.readBoolEntry("History", "SaveStatusChanges") || (!elem.data("history_save_status").toBool() && !config_file.readBoolEntry("History", "SaveStatusChangesForAll")) || (config_file.readBoolEntry("History", "IgnoreSomeStatusChanges") && ((elem.status(protocolName).isOnline() && oldStatus.isBusy()) || (elem.status(protocolName).isBusy() && oldStatus.isOnline()))) || (!elem.status(protocolName).hasDescription() && config_file.readBoolEntry("History", "SaveOnlyStatusWithDescription")) || elem.status(protocolName) == oldStatus && elem.status(protocolName).description() != oldStatus.description()) 
	{
		kdebugm(KDEBUG_INFO|KDEBUG_FUNCTION_END, "not appending\n");
		return;
	}
	appendStatus(elem, protocolName);
	kdebugf2();
}

void SqlHistory::appendStatus(UserListElement elem, QString protocolName)
{
	kdebugf();
	QHostAddress ip;
	unsigned short port;
	QString desc, query_str;
	if(debe.driverName() == "QSQLITE")
		query_str  = "INSERT INTO kadu_status (uid_group_id, status, time, description, ip_port) VALUES ('%1', '%2', datetime('now', 'localtime'), '%3', '%4');";
	else if(debe.driverName() == "QMYSQL" || debe.driverName() == "QPSQL")
		query_str  = "INSERT INTO kadu_status (uid_group_id, status, time, description, ip_port) VALUES ('%1', '%2', NOW(), '%3', '%4');";
	QString uid_group_str = findUidGroup(UserListElements(elem));
	if (uid_group_str == "")
		uid_group_str = addUidGroup(UserListElements(elem));
	if (uid_group_str == "")
	{
		kdebugf2();
		return;
	}
	if (userlist->contains(protocolName, elem.ID(protocolName), FalseForAnonymous))
	{
		UserListElement user = userlist->byID(protocolName, elem.ID(protocolName));
		ip = user.IP(protocolName);
		port = user.port(protocolName);
	}
	else
	{
		ip.setAddress((unsigned int)0);
		port = 0;
	}
	QString addr = ip.toString();
	if (port)
		addr = addr + QString(":") + QString::number(port);
	if (elem.status(protocolName).hasDescription())
	{
		desc = elem.status(protocolName).description();
		HtmlDocument::escapeText(desc);
	}

	query_str = query_str.arg(uid_group_str).arg(QString(elem.status(protocolName).toString().toUtf8().data())).arg(QString(desc.toUtf8().data())).arg(QString(addr.toUtf8().data()));
	executeQuery(query_str);
	kdebugf2();
}

bool SqlHistory::beginTransaction()
{
	kdebugf();
	if (!debe.transaction())
		return false;
	return true;
	kdebugf2();
}

bool SqlHistory::commitTransaction()
{
	kdebugf();
	if (!debe.commit())
		return false;
	return true;
	kdebugf2();
}

bool SqlHistory::rollbackTransaction()
{
	kdebugf();
	if (!debe.rollback())
		return false;
	return true;
	kdebugf2();
}

QString SqlHistory::findUidGroup(UserListElements users)
{
	kdebugf();
	QSqlQuery query(debe);
	QString query_str = "SELECT t0.id FROM";
	for (unsigned int i = 0; i < (unsigned int)users.count(); i++)
	{
		if (i > 0)
			query_str += ",";
		query_str += (" kadu_uid_groups t" + QString::number(i));
	}
	query_str += " WHERE ";
	for (unsigned int i = 0; i < (unsigned int)users.count(); i++)
	{
		if (i > 0)
			query_str += " and ";
		query_str += ("t" + QString::number(i) + ".uid = '" + users[i].ID((*users[i].protocolList().begin())) + "' AND t" + QString::number(i) + ".protocol = '" + (*users[i].protocolList().begin()) + "'");
	}
	for (unsigned int i = 0; i < (unsigned int)users.count() - 1; i++)
		query_str += (" and t" + QString::number(i) + ".id = t" + QString::number(i + 1) + ".id");
	kdebug("query: %s\n", query_str.local8Bit().data());
	if (!query.exec(query_str))
	{
		MessageBox::msg(query.lastError().text(), false, "Warning");
		kdebugf2();
		return "";
	}
	if (!query.next())
	{
		kdebugf2();
		return "";
	}
	if (query.value(0).isNull())
	{
		kdebugf2();
		return "";
	}
	QString result = query.value(0).asString();
	kdebugf2();
	return result;
}

QString SqlHistory::findNewUidGroupId()
{
	kdebugf();
	QSqlQuery query(debe);
	QString query_str = "SELECT MAX(id) FROM kadu_uid_groups";
	if (!query.exec(query_str))
	{
		MessageBox::msg(query.lastError().text(), false, "Warning");
		kdebugf2();
		return "";
	}
	if (!query.next())
	{
		kdebugf2();
		return "";
	}
	if (query.value(0).isNull())
	{
		kdebugf2();
		return "0";
	}
	QString result = QString::number(query.value(0).toInt() + 1);
	kdebugf2();
	return result;
}

QString SqlHistory::addUidGroup(UserListElements users)
{
	kdebugf();
	QString id_str = findNewUidGroupId();
	if (id_str == "")
	{
		kdebugf2();
		return "";
	}
	kdebug("adding uid group %s\n", id_str.local8Bit().data());
	for (unsigned int i = 0; i < (unsigned int)users.count(); i++)
	{
		QString query_str = "INSERT INTO kadu_uid_groups (id, protocol, uid) VALUES('%1', '%2', '%3')";
		QString uid_str = users[i].ID((*users[i].protocolList().begin()));
		QString proto_str = (*users[i].protocolList().begin());
		query_str = query_str.arg(id_str).arg(proto_str).arg(uid_str);
		executeQuery(query_str);
	}
	kdebugf2();
	return id_str;
}

void SqlHistory::viewHistory(QAction *sender, bool toggled)
{
	kdebugf();
	KaduMainWindow *window = dynamic_cast<KaduMainWindow *>(sender->parent());
	if (window)
	{
		UserListElements users = window->userListElements();
		//historyDialog = new HistoryDlg(users);
		historyDialog->showDialog(users);
	}
	
	kdebugf2();
}

void SqlHistory::chatMsgReceived(Protocol *protocol, UserListElements senders,
	const QString& msg, time_t time)
{
	kdebugf();
	if (!config_file.readBoolEntry("History", "Enable") || !config_file.readBoolEntry("History", "SaveChats"))
	{
		kdebugm(KDEBUG_INFO|KDEBUG_FUNCTION_END, "not appending\n");
		return;
	}
	foreach(const UserListElement &sender, senders)
		if ((config_file.readBoolEntry("History", "DontSaveChatsWithAnonymous") && sender.isAnonymous()) || (!sender.data("history_save_chats").toBool() && !config_file.readBoolEntry("History", "SaveChatsForAll")))
		{
			kdebugm(KDEBUG_INFO|KDEBUG_FUNCTION_END, "not appending\n");
			return;
		} 
	QString query_str;
	if(debe.driverName() == "QSQLITE")
		query_str = "INSERT INTO kadu_messages (uid_group_id, is_outgoing, send_time, receive_time, content) VALUES ('%1', 0, datetime('%2', 'unixepoch', 'localtime'), datetime('now', 'localtime'), '%3');";
	else if(debe.driverName() == "QMYSQL" || debe.driverName() == "QPSQL")
		query_str = "INSERT INTO kadu_messages (uid_group_id, is_outgoing, send_time, receive_time, content) VALUES ('%1', '0', FROM_UNIXTIME('%2'), NOW(), '%3');";
	QString uid_group_str = findUidGroup(senders);
	if (uid_group_str == "")
		uid_group_str = addUidGroup(senders);
	if (uid_group_str == "")
	{
		kdebugf2();
		return;
	}
	QString send_time_str = QString::number(time);
	QString message_str = prepareText(msg);
	query_str = query_str.arg(uid_group_str).arg(send_time_str).arg(QString(message_str.toUtf8()));

	executeQuery(query_str);
	kdebugf2();
}

void SqlHistory::messageSentAndConfirmed(UserListElements receivers, const QString& message, time_t time)
{
	kdebugf();
	if (!config_file.readBoolEntry("History", "Enable") || !config_file.readBoolEntry("History", "SaveChats"))
	{
		kdebugm(KDEBUG_INFO|KDEBUG_FUNCTION_END, "not appending\n");
		return;
	}
	foreach(const UserListElement &receiver, receivers)
		if ((config_file.readBoolEntry("History", "DontSaveChatsWithAnonymous") && receiver.isAnonymous()) || (!receiver.data("history_save_chats").toBool() && !config_file.readBoolEntry("History", "SaveChatsForAll")))
		{
			kdebugm(KDEBUG_INFO|KDEBUG_FUNCTION_END, "not appending\n");
			return;
		} 
	QString query_str;
	if(debe.driverName() == "QSQLITE")
		query_str = "INSERT INTO kadu_messages (uid_group_id, is_outgoing, send_time, receive_time, content) VALUES ('%1', '1', datetime('now', 'localtime'), datetime('now', 'localtime'), '%2');";
	else if(debe.driverName() == "QMYSQL" || debe.driverName() == "QPSQL")
		query_str = "INSERT INTO kadu_messages (uid_group_id, is_outgoing, send_time, receive_time, content) VALUES ('%1', '1', NOW(), NOW(), '%2');";
	QString uid_group_str = findUidGroup(receivers);
	if (uid_group_str == "")
		uid_group_str = addUidGroup(receivers);
	if (uid_group_str == "")
	{
		kdebugf2();
		return;
	}
	QString message_str = prepareText(message);
	query_str = query_str.arg(uid_group_str).arg(QString(message_str.toUtf8()));
	executeQuery(query_str);
	kdebugf2();
}

void SqlHistory::appendMessageEntry(UserListElements list, const QString &msg, bool outgoing, time_t send_time, time_t receive_time)
{
	QString query_str;
	if(debe.driverName() == "QSQLITE")
		query_str = "INSERT INTO kadu_messages (uid_group_id, is_outgoing, send_time, receive_time, content) VALUES ('%1', '%2', datetime('%3', 'unixepoch', 'localtime'), datetime('%4', 'unixepoch', 'localtime'), '%5');";
	else if(debe.driverName() == "QMYSQL" || debe.driverName() == "QPSQL")
		query_str = "INSERT INTO kadu_messages (uid_group_id, is_outgoing, send_time, receive_time, content) VALUES ('%1', '%2', FROM_UNIXTIME('%3'), FROM_UNIXTIME('%4'), '%5');";

	QString uid_group_str = findUidGroup(list);
	if (uid_group_str == "")
		uid_group_str = addUidGroup(list);
	if (uid_group_str == "")
	{
		kdebugf2();
		return;
	}
	QString send_time_str = QString::number(send_time);
	QString rec_time_str = QString::number(receive_time);
	QString outstr = QString::number(outgoing);
	QString message_str = prepareText(msg);
	query_str = query_str.arg(uid_group_str).arg(outstr).arg(send_time_str).arg(rec_time_str).arg(QString(message_str.toUtf8()));

	executeQuery(query_str);
}

void SqlHistory::appendSmsEntry(UserListElements list, const QString &msg, bool outgoing, time_t send_time, time_t receive_time)
{
	kdebugf();
	QString query_str;
	if(debe.driverName() == "QSQLITE")
		query_str = "INSERT INTO kadu_sms (uid_group_id, is_outgoing, send_time, receive_time, content) VALUES ('%1', '%2', datetime('%3', 'unixepoch', 'localtime'), datetime('%4', 'unixepoch', 'localtime'), '%5');";
	else if(debe.driverName() == "QMYSQL" || debe.driverName() == "QPSQL")
		query_str = "INSERT INTO kadu_sms (uid_group_id, is_outgoing, send_time, receive_time, content) VALUES ('%1', '%2', FROM_UNIXTIME('%3'), FROM_UNIXTIME('%4'), '%5');";

	QString uid_group_str = findUidGroup(list);
	if (uid_group_str == "")
		uid_group_str = addUidGroup(list);
	if (uid_group_str == "")
	{
		kdebugf2();
		return;
	}
	QString message_str = prepareText(msg);
	QString outstr = QString::number(outgoing);
	QString send_time_str = QString::number(send_time);
	QString rec_time_str = QString::number(receive_time);
	query_str = query_str.arg(uid_group_str).arg(outstr).arg(send_time_str).arg(rec_time_str).arg(QString(message_str.toUtf8()));
	executeQuery(query_str);
	kdebugf2();
}


void SqlHistory::appendStatusEntry(UserListElements list, const QString &status, const QString &desc, time_t time, const QString &ip_port)
{
	kdebugf();
	QString query_str;
	if(debe.driverName() == "QSQLITE")
		query_str  = "INSERT INTO kadu_status (uid_group_id, status, time, description, ip_port) VALUES ('%1', '%2', datetime('%3', 'unixepoch', 'localtime'), '%4', '%5');";
	else if(debe.driverName() == "QMYSQL" || debe.driverName() == "QPSQL")
		query_str  = "INSERT INTO kadu_status (uid_group_id, status, time, description, ip_port) VALUES ('%1', '%2', FROM_UNIXTIME('%3'), '%4', '%5');";
	QString uid_group_str = findUidGroup(list);
	if (uid_group_str == "")
		uid_group_str = addUidGroup(list);
	if (uid_group_str == "")
	{
		kdebugf2();
		return;
	}
	query_str = query_str.arg(uid_group_str).arg(status).arg(QString::number(time)).arg(QString(desc.toUtf8().data())).arg(ip_port.toUtf8().data());
	executeQuery(query_str);
	kdebugf2();


}

void SqlHistory::appendSms(const QString &mobile, const QString &msg)
{
	kdebugf();

	QString query_str;
	if(debe.driverName() == "QSQLITE")
		query_str = "INSERT INTO kadu_sms (uid_group_id, is_outgoing, send_time, receive_time, content) VALUES ('%1', '1', datetime('now', 'localtime'), datetime('now', 'localtime'), '%2');";
	else if(debe.driverName() == "QMYSQL" || debe.driverName() == "QPSQL")
		query_str = "INSERT INTO kadu_sms (uid_group_id, is_outgoing, send_time, receive_time, content) VALUES ('%1', '1', NOW(), NOW(), '%2');";
	UserListElement mob = UserListElement();
	mob.addProtocol("SMS", mobile);
	UserListElements mobs = UserListElements(mob);
	QString uid_group_str = findUidGroup(mobs);
	if (uid_group_str == "")
		uid_group_str = addUidGroup(mobs);
	if (uid_group_str == "")
	{
		kdebugf2();
		return;
	}
	QString message_str = prepareText(msg);
	query_str = query_str.arg(uid_group_str).arg(QString(message_str.toUtf8()));

	executeQuery(query_str);
	kdebugf2();
}

void SqlHistory::removeHistory(const UserListElements& uids, const QDate &date, HistoryEntryType type)
{
	kdebugf();
	QString uid_group_str = findUidGroup(uids);
	if(date.isValid())
	{
		if(type == EntryTypeMessage)
		{
			QString query_str = "DELETE FROM kadu_messages WHERE uid_group_id='%1' AND date(receive_time) = date('%2');";
			query_str = query_str.arg(uid_group_str).arg(date.toString(Qt::ISODate));
			executeQuery(query_str);
		}
		else if(type == EntryTypeStatus)
		{
			QString query_str = "DELETE FROM kadu_status WHERE uid_group_id='%1' AND date(time) = date('%2');";
			query_str = query_str.arg(uid_group_str).arg(date.toString(Qt::ISODate));
			executeQuery(query_str);
		}
		else if(type == EntryTypeSms)
		{
			QString query_str = "DELETE FROM kadu_sms WHERE uid_group_id='%1' AND date(receive_time) = date('%2');";
			query_str = query_str.arg(uid_group_str).arg(date.toString(Qt::ISODate));
			executeQuery(query_str);
		}
	}
	else
	{
		//rozmowy
		QString query_str = "DELETE FROM kadu_messages WHERE uid_group_id='%1';";
		query_str = query_str.arg(uid_group_str);
		executeQuery(query_str);
		//statusy
		query_str = "DELETE FROM kadu_status WHERE uid_group_id='%1';";
		query_str = query_str.arg(uid_group_str);
		executeQuery(query_str);
		//sms'y
		query_str = "DELETE FROM kadu_sms WHERE uid_group_id='%1';";
		query_str = query_str.arg(uid_group_str);
		executeQuery(query_str);
		//i posprz±taj po sobie
		query_str = "DELETE FROM kadu_uid_groups WHERE id='%1';";
		query_str = query_str.arg(uid_group_str);
		executeQuery(query_str);
	}
	kdebugf2();

}

void SqlHistory::historyActionActivated(QAction* sender, bool toggled)
{
	kdebugf();
	KaduMainWindow *window = dynamic_cast<KaduMainWindow *>(sender->parent());
	if (window)
	{
		//historyDialog = new HistoryDlg(window->userListElements());
		historyDialog->showDialog(window->userListElements());
	}
	kdebugf2();
}


void SqlHistory::configurationUpdated()
{
	kdebugf();
	initializeDatabase();
	kdebugf2();
}


QList<UserListElements> SqlHistory::getAllUidGroups()
{
	kdebugf();
	return getUidGroups("SELECT id, protocol, uid FROM kadu_uid_groups;");
	kdebugf2();
}

QList<UserListElements> SqlHistory::getChatUidGroups()
{
	kdebugf();
	return getUidGroups("SELECT id as aj, protocol, uid FROM kadu_uid_groups WHERE (SELECT content FROM kadu_messages WHERE uid_group_id=aj LIMIT 1) != '';");
	kdebugf2();
}

QList<UserListElements> SqlHistory::getStatusUidGroups()
{
	kdebugf();
	return getUidGroups("SELECT id as aj, protocol, uid FROM kadu_uid_groups WHERE (SELECT status FROM kadu_status WHERE uid_group_id=aj LIMIT 1) != '';");
	kdebugf2();
}

QList<UserListElements> SqlHistory::getSmsUidGroups()
{
	kdebugf();
	return getUidGroups("SELECT id as aj, protocol, uid FROM kadu_uid_groups WHERE (SELECT content FROM kadu_sms WHERE uid_group_id=aj LIMIT 1) != '';");
	kdebugf2();
}

QList<UserListElements> SqlHistory::getUidGroups(QString queryString)
{
	kdebugf();
	QList<UserListElements> result;
	QSqlQuery query(debe);
	if (!query.exec(queryString))
	{
		MessageBox::msg(query.lastError().text(), false, "Warning");
		kdebugf2();
		return result;
	}
	int prev_id = -1;
	UserListElements user_list_elements;
	while (query.next())
	{
		int id = query.value(0).asInt();
		QString protocol = query.value(1).asString();
		QString uid = query.value(2).asString();
		UserListElement user_list_element;
		user_list_element.addProtocol(protocol, uid);
		if(userlist->contains(protocol, uid))
		{
			user_list_element.setAnonymous(false);
			user_list_element.setAltNick(userlist->byID(protocol,uid).altNick());
		}
		else
		{
			user_list_element.setAnonymous(true);
			user_list_element.setAltNick(uid);
		}
		if (id != prev_id && prev_id != -1)
		{
			result.append(user_list_elements);
			user_list_elements.clear();
		}
		user_list_elements.append(user_list_element);
		prev_id = id;
	}
	if (prev_id != -1)
	{
		result.append(user_list_elements);
		user_list_elements.clear();
	}
	kdebug("%i uid groups\n", result.count());
	kdebugf2();
	return result;
}



QList<QDate> SqlHistory::getAllDates()
{
	kdebugf();
	QList<QDate> result;
	QSqlQuery query(debe);
	QString query_str = "SELECT DISTINCT date(send_time) FROM kadu_messages";
	kdebug("query: %s\n", query_str.local8Bit().data());
	if (!query.exec(query_str))
	{
		MessageBox::msg(query.lastError().text(), false, "Warning");
		kdebugf2();
		return result;
	}
	while (query.next())
	{
		QDate date = query.value(0).asDate();
		result.append(date);
	}
	kdebug("%i dates\n", result.count());
	kdebugf2();
	return result;
}


QList<QDate> SqlHistory::historyDates(const UserListElements& uids)
{
	kdebugf();
	QList<QDate> result;
	QSqlQuery query(debe);
	QString query_str = "SELECT DISTINCT date(send_time) FROM kadu_messages WHERE uid_group_id = '%1'";
	QString uid_group_str = findUidGroup(uids);
	if (uid_group_str == "")
	{
		kdebugf2();
		return result;
	}
	query_str = query_str.arg(uid_group_str);
	kdebug("query: %s\n", query_str.local8Bit().data());
	if (!query.exec(query_str))
	{
		MessageBox::msg(query.lastError().text(), false, "Warning");
		kdebugf2();
		return result;
	}
	while (query.next())
	{
		QDate date = query.value(0).asDate();
		result.append(date);
	}
	kdebug("%i dates\n", result.count());
	kdebugf2();
	return result;
}

QList<QDate> SqlHistory::historyStatusDates(const UserListElements& uids)
{
	kdebugf();
	QList<QDate> result;
	QSqlQuery query(debe);
	QString query_str = "SELECT DISTINCT date(time) FROM kadu_status WHERE uid_group_id = '%1'";
	QString uid_group_str = findUidGroup(uids);
	if (uid_group_str == "")
	{
		kdebugf2();
		return result;
	}
	query_str = query_str.arg(uid_group_str);
	kdebug("query: %s\n", query_str.local8Bit().data());
	if (!query.exec(query_str))
	{
		MessageBox::msg(query.lastError().text(), false, "Warning");
		kdebugf2();
		return result;
	}
	while (query.next())
	{
		QDate date = query.value(0).asDate();
		result.append(date);
	}
	kdebug("%i dates\n", result.count());
	kdebugf2();
	return result;
}

QList<QDate> SqlHistory::historySmsDates(const UserListElements& uids)
{
	kdebugf();
	QList<QDate> result;
	QSqlQuery query(debe);
	QString query_str = "SELECT DISTINCT date(send_time) FROM kadu_sms WHERE uid_group_id = '%1'";
	QString uid_group_str = findUidGroup(uids);
	if (uid_group_str == "")
	{
		kdebugf2();
		return result;
	}
	query_str = query_str.arg(uid_group_str);
	kdebug("query: %s\n", query_str.local8Bit().data());
	if (!query.exec(query_str))
	{
		MessageBox::msg(query.lastError().text(), false, "Warning");
		kdebugf2();
		return result;
	}
	while (query.next())
	{
		QDate date = query.value(0).asDate();
		result.append(date);
	}
	kdebug("%i dates\n", result.count());
	kdebugf2();
	return result;
}

QList<ChatMessage*> SqlHistory::historyMessages(const UserListElements& uids, QDate date)
{
	kdebugf();
	QList<ChatMessage*> result;
	QSqlQuery query(debe);
	QString query_str;
	/*if(toDate)
		query_str = "SELECT uid_group_id, is_outgoing, send_time, receive_time, content FROM kadu_messages WHERE uid_group_id = '%1' AND date(receive_time) <= date('%2');";
	else */if(date.isNull())
		query_str = "SELECT uid_group_id, is_outgoing, send_time, receive_time, content FROM kadu_messages WHERE uid_group_id = '%1' ORDER BY receive_time ASC;";
	else
		query_str = "SELECT uid_group_id, is_outgoing, send_time, receive_time, content FROM kadu_messages WHERE uid_group_id = '%1' AND date(receive_time) = date('%2');";
	QString uid_group_str = findUidGroup(uids);
	if (uid_group_str == "")
	{
		kdebugf2();
		return result;
	}
	if(date.isNull())
		query_str = query_str.arg(uid_group_str);
	else
	{
		QString recv_time_str = date.toString(Qt::ISODate);
		query_str = query_str.arg(uid_group_str).arg(recv_time_str);
	}
	kdebug("query: %s\n", query_str.local8Bit().data());
	if (!query.exec(query_str))
	{
		MessageBox::msg(query.lastError().text(), false, "Warning");
		kdebugf2();
		return result;
	}
	while (query.next())
	{
		bool outgoing = query.value(1).asBool();
		QDateTime send_time = query.value(2).asDateTime();
		QDateTime receive_time = query.value(3).asDateTime();
		QString message = query.value(4).asString();

		ChatMessage* chat_message;

		if(outgoing)
			chat_message = new ChatMessage(kadu->myself(), uids, QString::fromUtf8(message), TypeSent, receive_time, send_time);
		else
			chat_message = new ChatMessage((*uids.begin()), UserListElements(kadu->myself()), QString::fromUtf8(message), TypeReceived, receive_time, send_time);

		result.append(chat_message);
	}
	
	kdebug("%i messages\n", result.count());
	kdebugf2();
	return result;
}

QList<ChatMessage*> SqlHistory::getStatusEntries(const UserListElements& uids, QDate date)
{
	kdebugf();
	QList<ChatMessage*> result;
	QSqlQuery query(debe);
	QString query_str;
	if(!date.isValid())
		query_str = "SELECT status, time, description, ip_port FROM kadu_status WHERE uid_group_id = '%1' ORDER BY time ASC;";
	else
		query_str = "SELECT status, time, description, ip_port FROM kadu_status WHERE uid_group_id = '%1' AND date(time) = date('%2');";
	QString uid_group_str = findUidGroup(uids);
	if (uid_group_str == "")
	{
		kdebugf2();
		return result;
	}
	if(!date.isValid())
		query_str = query_str.arg(uid_group_str);
	else
	{
		QString recv_time_str = date.toString(Qt::ISODate);
		query_str = query_str.arg(uid_group_str).arg(recv_time_str);
	}
	kdebug("query: %s\n", query_str.local8Bit().data());
	if (!query.exec(query_str))
	{
		MessageBox::msg(query.lastError().text(), false, "Warning");
		kdebugf2();
		return result;
	}
	while (query.next())
	{
		QDateTime send_time = query.value(1).asDateTime();
		QDateTime receive_time = query.value(1).asDateTime();
		QString message = query.value(0).asString();
		UserStatus us = UserStatus();
		us.fromString(query.value(0).asString());
		us.setDescription(query.value(2).asString());
		QStringList address = query.value(3).asString().split(':');
		UserListElement user = (*uids.begin());
		if(!address.isEmpty())
			if(address.count() == 1)
				user.setAddressAndPort("Gadu", QHostAddress(address.at(0)), 0);
			else if(address.count() == 2)
				user.setAddressAndPort("Gadu", QHostAddress(address.at(0)), address.at(1).toUInt());
		user.setStatus("Gadu", us, true, false);
		ChatMessage* chat_message = new ChatMessage(user, UserListElements(kadu->myself()), QString::fromUtf8(message + " : " + query.value(2).asString()), TypeReceived, receive_time, send_time);
		result.append(chat_message);
	}
	
	kdebug("%i status entries\n", result.count());
	kdebugf2();
	return result;
}

QList<ChatMessage*> SqlHistory::getSmsEntries(const UserListElements& uids, QDate date)
{
	kdebugf();
	QList<ChatMessage*> result;
	QSqlQuery query(debe);
	QString query_str;

	if(date.isValid())
		query_str = "SELECT send_time, content FROM kadu_sms WHERE uid_group_id = '%1' ORDER BY send_time ASC;";
	else
		query_str = "SELECT send_time, content FROM kadu_sms WHERE uid_group_id = '%1' AND date(send_time) = date('%2') ORDER BY send_time ASC;";
	QString uid_group_str = findUidGroup(uids);
	if (uid_group_str == "")
	{
		kdebugf2();
		return result;
	}
	if(date.isNull())
		query_str = query_str.arg(uid_group_str);
	else
	{
		QString recv_time_str = date.toString(Qt::ISODate);
		query_str = query_str.arg(uid_group_str).arg(recv_time_str);
	}

	kdebug("query: %s\n", query_str.local8Bit().data());
	if (!query.exec(query_str))
	{
		MessageBox::msg(query.lastError().text(), false, "Warning");
		kdebugf2();
		return result;
	}
	while (query.next())
	{
		QDateTime send_time = query.value(0).asDateTime();
		QString message = query.value(1).asString();
		ChatMessage* chat_message;
			chat_message = new ChatMessage(kadu->myself(), uids, QString::fromUtf8(message), TypeSent, send_time, send_time);

		result.append(chat_message);
	}
	
	kdebug("%i status entries\n", result.count());
	kdebugf2();
	return result;
}


int SqlHistory::getEntriesCount(const QList<UserListElements> &uids, HistoryEntryType type)
{
	kdebugf();
	int result = 0;
	QSqlQuery query(debe);
	QString query_str;

	foreach(const UserListElements uid, uids)
{
	if(type == EntryTypeMessage)
	{

		QString query_str = "SELECT COUNT(uid_group_id) FROM kadu_messages WHERE uid_group_id = '%1';";
			
	QString uid_group_str = findUidGroup(uid);
	if (uid_group_str == "")
	{
		kdebugf2();
		return 0;
	}
	query_str = query_str.arg(uid_group_str);
	kdebug("query: %s\n", query_str.local8Bit().data());
	if (!query.exec(query_str))
	{
		MessageBox::msg(query.lastError().text(), false, "Warning");
		kdebugf2();
		return 0;
	}
	query.next();
	int part = query.value(0).asInt();
	result +=part;
	}
	else if(type == EntryTypeStatus)
	{
			QString query_str = "SELECT COUNT(uid_group_id) FROM kadu_status WHERE uid_group_id = '%1';";
	QString uid_group_str = findUidGroup(uid);
	if (uid_group_str == "")
	{
		kdebugf2();
		return 0;
	}
	query_str = query_str.arg(uid_group_str);
	kdebug("query: %s\n", query_str.local8Bit().data());
	if (!query.exec(query_str))
	{
		MessageBox::msg(query.lastError().text(), false, "Warning");
		kdebugf2();
		return 0;
	}
	query.next();
	int part = query.value(0).asInt();
	result +=part;
	}	
	else if(type == EntryTypeSms)
	{
			QString query_str = "SELECT COUNT(uid_group_id) FROM kadu_sms WHERE uid_group_id = '%1';";
	QString uid_group_str = findUidGroup(uid);
	if (uid_group_str == "")
	{
		kdebugf2();
		return 0;
	}
	query_str = query_str.arg(uid_group_str);
	kdebug("query: %s\n", query_str.local8Bit().data());
	if (!query.exec(query_str))
	{
		MessageBox::msg(query.lastError().text(), false, "Warning");
		kdebugf2();
		return 0;
	}
	query.next();
	int part = query.value(0).asInt();
	result +=part;
	}	
	else if(type == EntryTypeAll)
	{
		int part = 0;
			QString query_str = "SELECT COUNT(uid_group_id) FROM kadu_messages WHERE uid_group_id = '%1';";
			QString uid_group_str = findUidGroup(uid);
	if (uid_group_str == "")
	{
		kdebugf2();
		return 0;
	}
	query_str = query_str.arg(uid_group_str);
	kdebug("query: %s\n", query_str.local8Bit().data());
	if (!query.exec(query_str))
		{
			MessageBox::msg(query.lastError().text(), false, "Warning");
			kdebugf2();
			return 0;
		}
		query.next();
		int sub = query.value(0).asInt();
		part += sub;

			query_str = "SELECT COUNT(uid_group_id) FROM kadu_status WHERE uid_group_id = '%1';";
			uid_group_str = findUidGroup(uid);
	if (uid_group_str == "")
	{
		kdebugf2();
		return 0;
	}
	query_str = query_str.arg(uid_group_str);
	kdebug("query: %s\n", query_str.local8Bit().data());
	if (!query.exec(query_str))
		{
			MessageBox::msg(query.lastError().text(), false, "Warning");
			kdebugf2();
			return 0;
		}
		query.next();
		sub = query.value(0).asInt();
		part += sub;

			query_str = "SELECT COUNT(uid_group_id) FROM kadu_sms WHERE uid_group_id = '%1';";
	uid_group_str = findUidGroup(uid);
	if (uid_group_str == "")
	{
		kdebugf2();
		return 0;
	}
	query_str = query_str.arg(uid_group_str);
	kdebug("query: %s\n", query_str.local8Bit().data());
	if (!query.exec(query_str))
		{
			MessageBox::msg(query.lastError().text(), false, "Warning");
			kdebugf2();
			return 0;
		}
		sub = query.value(0).asInt();
		part += sub;
		result +=part;
	}	
}
kdebugf2();
return result;
	
}


void SqlHistory::appendHistory(ChatWidget *chat)
{
	kdebugf();
	unsigned int chatHistoryQuotation = config_file.readUnsignedNumEntry("History", "ChatHistoryCitation");
	int quotTime = config_file.readNumEntry("History","ChatHistoryQuotationTime");
	QList<ChatMessage *> messages;
	QSqlQuery query(debe);
	QString query_str = "SELECT is_outgoing, send_time, receive_time, content, uid_group_id FROM kadu_messages WHERE uid_group_id = '%1' AND '%2' - receive_time <= '%3' ORDER BY receive_time DESC LIMIT %4;";

	QString uid_group_str = findUidGroup(chat->users()->toUserListElements());
	if (uid_group_str == "")
	{
		kdebugf2();
		return;
	}
	QString recv_time_str = QDateTime::currentDateTime().date().toString(Qt::ISODate);
	query_str = query_str.arg(uid_group_str).arg(recv_time_str).arg(QString::number(-quotTime * 3600)).arg(QString::number(chatHistoryQuotation));
	kdebug("query: %s\n", query_str.local8Bit().data());
	if (!query.exec(query_str))
	{
		MessageBox::msg(query.lastError().text(), false, "Warning");
		kdebugf2();
		return;
	}
	while (query.next())
	{
		bool outgoing = query.value(0).asBool();
		QDateTime send_time = query.value(1).asDateTime();
		QDateTime receive_time = query.value(2).asDateTime();
		QString message = query.value(3).asString();
		QSqlQuery uidquery(debe);
		QString uidquerystr = "SELECT uid, protocol FROM kadu_uid_groups WHERE id = '%1' LIMIT 1;";
		uidquerystr = uidquerystr.arg(query.value(4).asString());
		if (!uidquery.exec(uidquerystr))
		{
			MessageBox::msg(uidquery.lastError().text(), false, "Warning");
			kdebugf2();
			return;
		}
		uidquery.next();
		ChatMessage* chat_message;
		if(outgoing)
			chat_message = new ChatMessage(kadu->myself(), chat->users()->toUserListElements(), QString::fromUtf8(message), TypeSent, receive_time, send_time);
		else
			chat_message = new ChatMessage((*chat->users()->toUserListElements().begin()), UserListElements(kadu->myself()), QString::fromUtf8(message), TypeReceived, receive_time, send_time);

		messages.prepend(chat_message);
	}
	kdebug("%i messages\n", messages.count());
	if(!messages.isEmpty())
		chat->appendMessages(messages);
	kdebugf2();
}

void SqlHistory::chatCreated(ChatWidget *chat)
{
	kdebugf();
	connect(chat, SIGNAL(keyPressed(QKeyEvent *, ChatWidget *, bool &)),
		this, SLOT(chatKeyPressed(QKeyEvent *, ChatWidget *, bool &)));

	connect(chat, SIGNAL(messageSentAndConfirmed(UserListElements, const QString&)),
		this, SLOT(messageSentAndConfirmed(UserListElements, const QString&)));

	// don't do it for already opened chats with discussions
	if (chat->countMessages() != 0)
		return;

	appendHistory(chat);
	kdebugf2();
}

void SqlHistory::chatDestroying(ChatWidget *chat)
{
	disconnect(chat, SIGNAL(keyPressed(QKeyEvent *, ChatWidget *, bool &)),
		this, SLOT(chatKeyPressed(QKeyEvent *, ChatWidget *, bool &)));
}

void SqlHistory::chatKeyPressed(QKeyEvent *e, ChatWidget *chatWidget, bool &handled)
{
	if (HotKey::shortCut(e, "ShortCuts", "kadu_viewhistory"))
	{
// 			UserBox *activeUserBox = UserBox::activeUserBox();
// 		if (activeUserBox == NULL)
// 		{
// 			kdebugf2();
// 			return;
// 		}
// 		UserListElements users = activeUserBox->selectedUsers();
// 		historyDialog = new HistoryDlg(chatWidget->users()->toUserListElements());
// 		historyDialog->show();
// 		handled = true;
// 		KaduMainWindow *window = dynamic_cast<KaduMainWindow *>(sender->parent());
// 		if (window)
// 		{
			
			historyDialog->showDialog(chatWidget->users()->toUserListElements());
			handled = true;
// 		}
	}
}

void SqlHistory::userboxMenuPopup()
{
	kdebugf();
///WTF? Potrzebne to jeszcze?
// 	UserBox *activeUserBox = UserBox::activeUserBox();
// 	if (activeUserBox == NULL)
// 	{
// 		kdebugf2();
// 		return;
// 	}
// 
// 	UserListElements users = activeUserBox->selectedUsers();
// 	if (!users.count())
// 	{
// 		kdebugf2();
// 		return;
// 	}

/*	int history_item = UserBox::userboxmenu->getItem(tr("History"));
	int delete_history_item = UserBox::management->getItem(tr("Clear history"));

	bool any_ok = false;
	foreach(const UserListElement &user, users)
		if (!user.protocolList().isEmpty() && (user.ID("Gadu") != kadu->myself().ID("Gadu")))
		{
			any_ok = true;
			break;
		}
	UserBox::userboxmenu->setItemVisible(history_item, any_ok);
	UserBox::userboxmenu->setItemVisible(delete_history_item, any_ok);*/

	kdebugf2();
}

void SqlHistory::createDefaultConfiguration()
{
	config_file.addVariable("History", "ChatHistoryCitation", 10);
	config_file.addVariable("History", "ChatHistoryQuotationTime", -24);
	config_file.addVariable("History", "EnableSaving", true);
	config_file.addVariable("History", "SaveChats", true);
	config_file.addVariable("History", "SaveChatsWithAnonymous", true);
	config_file.addVariable("History", "SaveStatusChanges", false);
	config_file.addVariable("History", "SaveStatusChangesForAll", false);
	config_file.addVariable("History", "IgnoreSomeStatusChanges", true);
	config_file.addVariable("History", "SaveOnlyStatusWithDescription", true);
	config_file.addVariable("ShortCuts", "kadu_viewhistory", "Ctrl+H");
	config_file.addVariable("History", "DatabaseDriver", "QSQLITE");
	config_file.addVariable("History", "DatabaseFilePath", ggPath("/history/history.db"));
	config_file.addVariable("History", "DatabaseTableNamePrefix", "kadu_");
	config_file.addVariable("History", "SaveUndeliveredMessages", true);
//TODO: by dodaæ resztê warto¶ci, je¶li jezzcze czego¶ brak

}

void SqlHistory::initializeDatabase()
{
	kdebugf();
	tableNamePrefix = config_file.readEntry("History", "DatabaseTableNamePrefix");
	QString driver = config_file.readEntry("History", "DatabaseDriver");
	if(!QSqlDatabase::isDriverAvailable(driver))
	{
		MessageBox::msg(tr("It seems your Qt library does not provide support for selected database.\n Please set another driver in configuration window or install library with '%1' plugin.").arg(driver), false, "Warning");
		return; 
	}	
	if(QSqlDatabase::contains("kadu-history"))
	{
		if(debe.isOpen())
			debe.close();
		QSqlDatabase::removeDatabase("kadu-history");
	}
	debe = QSqlDatabase::addDatabase(driver, "kadu-history");
	if(driver == "QSQLITE")
		debe.setDatabaseName(config_file.readEntry("History", "DatabaseFilePath"));
// 	else if(driver == "QMYSQL")
// 	{
// 		debe.setDatabaseName(config_file.readEntry("History", "DatabaseName"));	
// 		debe.setPort(config_file.readUnsignedNumEntry("History", "DatabaseHostPort"));
// 		debe.setHostName(config_file.readEntry("History", "DatabaseHost"));
// 		debe.setUserName(config_file.readEntry("History", "DatabaseUser"));
// 		debe.setPassword(pwHash(config_file.readEntry("History", "DatabasePassword")));
// 	}
	else if(driver == "QPSQL" || driver == "QMYSQL")
	{
		debe.setDatabaseName(config_file.readEntry("History", "DatabaseName"));	
		debe.setPort(config_file.readUnsignedNumEntry("History", "DatabaseHostPort"));
		debe.setHostName(config_file.readEntry("History", "DatabaseHost"));
		debe.setUserName(config_file.readEntry("History", "DatabaseUser"));
		debe.setPassword(pwHash(config_file.readEntry("History", "DatabasePassword")));
	}
	

	if(debe.open())
		kdebug("Connected to database, driver: %s\n", driver.local8Bit().data());
	else
	{
		MessageBox::msg(debe.lastError().text(), false, "Warning");
		return;
	}

	if (!debe.tables().contains(tableNamePrefix + "messages"))
	{
		QSqlQuery query(debe);
		QString querystr;
		if(debe.driverName() == "QSQLITE")
			querystr = "PRAGMA encoding = \"UTF-8\";";
		else if(debe.driverName() == "QMYSQL")
			querystr = "ALTER DATABASE `%1` DEFAULT CHARACTER SET utf8 COLLATE utf8_general_ci;";
		querystr = querystr.arg(debe.databaseName());
		executeQuery(querystr);
		executeQuery(QString("CREATE TABLE %1messages (uid_group_id INTEGER, account_id INTEGER, is_outgoing BOOLEAN, is_delivered BOOLEAN, is_encrypted BOOLEAN, send_time TIMESTAMP, receive_time TIMESTAMP, content TEXT);").arg(tableNamePrefix));
		executeQuery(QString("CREATE TABLE %1uid_groups (id INTEGER, account_id INTEGER, protocol VARCHAR(20), uid VARCHAR(255));").arg(tableNamePrefix));
		executeQuery(QString("CREATE TABLE %1status (uid_group_id INTEGER, status VARCHAR(255), time TIMESTAMP, description TEXT, ip_port TEXT);").arg(tableNamePrefix));
		executeQuery(QString("CREATE TABLE %1accounts (id INTEGER, protocol VARCHAR(20), uid VARCHAR(255));").arg(tableNamePrefix));
		executeQuery(QString("CREATE TABLE %1sms (uid_group_id INTEGER, account_id INTEGER, is_outgoing BOOLEAN, send_time TIMESTAMP, receive_time TIMESTAMP, content TEXT);").arg(tableNamePrefix));
	}

}

HistorySearchResult SqlHistory::searchHistory(UserListElements users, HistorySearchParameters params)
{
	kdebugf();
	int count = 0;
	QString title, tableName, query_str;
	HistorySearchResult result;
	result.users = users;
	result.currentType = params.currentType;
	QSqlQuery query(QSqlDatabase::database("kadu-history"));
	QString usersID = findUidGroup(users);
	if(params.currentType == EntryTypeStatus)
		tableName = tableNamePrefix + "status";
	else if(params.currentType == EntryTypeSms)
		tableName = tableNamePrefix + "sms";
	else
		tableName = tableNamePrefix + "messages";
	if(params.currentType == EntryTypeStatus)
		query_str = "SELECT time, description FROM " + tableName + " WHERE uid_group_id = :uid ";
	else
		query_str = "SELECT receive_time, content FROM " + tableName + " WHERE uid_group_id = :uid ";
	if (!params.fromDate.isNull() && !params.toDate.isNull())
   		query_str += "AND ( date(receive_time) BETWEEN ':fromdate' AND ':todate') ";
	else if (!params.fromDate.isNull() && params.toDate.isNull())
		query_str += "AND ( date(receive_time) > ':fromdate') ";
	else if (params.fromDate.isNull() && !params.toDate.isNull())
		query_str += "AND ( date(receive_time) < ':todate') ";
	
	if(params.currentType == EntryTypeStatus)
		//mo¿na by szukaæ i po statusie, ale jaki¶ problem z 'OR ... ' w zapytaniu - brak wyników wtedy
		query_str += "AND description LIKE :pattern"; //OR status like :pattern";
	else
		query_str += "AND content LIKE :pattern ";
	query.prepare(query_str);
	if (!params.fromDate.isNull())
		query.bindValue(":fromdate", params.fromDate.toString(Qt::ISODate));
	if (!params.toDate.isNull())
		query.bindValue(":todate", params.toDate.toString(Qt::ISODate));
	query.bindValue(":uid", usersID);
	if(params.wholeWordsSearch)
		query.bindValue(":pattern", "% " + params.pattern + " %");
	else 
		query.bindValue(":pattern", "%" + params.pattern + "%");
	if (!query.exec())
	{
		MessageBox::msg(query.lastError().text(), false, "Warning");
		kdebugf2();
	}
	kdebug("query: %s\n", query.executedQuery().local8Bit().data());
	while(query.next())
	{	
		title = QString::fromUtf8(query.value(1).asString());
		title.truncate(40);
		title += " ...";
		QDate recvDate = query.value(0).asDate();
		HistorySearchDetailsItem d = HistorySearchDetailsItem((*users.begin()).altNick(), title, recvDate, /**length*/ 1);
		if(!result.detailsItems.isEmpty())
		{
			if(recvDate == result.detailsItems.last().date)
				++result.detailsItems.last().length;
			else
				result.detailsItems.append(d);
		}
		else
			result.detailsItems.append(d);
		//++count;
	}
	result.pattern = params.pattern;
	return result;
	kdebugf2();
}

void SqlHistory::removingUsers(UserListElements users)
{
	kdebugf();
	if (!MessageBox::ask(tr("The following users were deleted:\n%0\nDo you want to remove history as well?").arg(users.altNicks().join(", ")), "Warning", kadu))
		return;

	removeHistory(users);
	kdebugf2();
}

void SqlHistory::deleteHistory(const UserListElements &users)
{
	kdebugf();
	if (!MessageBox::ask(tr("You want to remove all history of selected users.\nAre you sure?\n")))
		return;
	removeHistory(users);
	kdebugf2();
}

QString SqlHistory::prepareText(const QString &text)
{
	QString str = text;
	str.replace('\'', "\'\'");
	str.replace('\\', "\\\\");
	return str;
}

void SqlHistory::executeQuery(const QString &query)
{
	kdebugf();
	QSqlQuery sqlQuery(debe);
	if (!sqlQuery.exec(query))
	{
		MessageBox::msg(sqlQuery.lastError().text(), false, "Warning");
		return;
	} 
	kdebug("query: %s\n", query.local8Bit().data());
	kdebugf2();
}

SqlHistory* sql_history = NULL;
