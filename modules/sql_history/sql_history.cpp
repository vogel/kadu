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

#include <QtCore/QString>
#include <QtGui/QLabel>
#include <QtGui/QGridLayout>
#include <QtNetwork/QHostAddress>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>

#include "accounts/account.h"
#include "accounts/account-manager.h"
#include "action.h"
// #include "chat_manager-old.h"
#include "debug.h"
#include "icons_manager.h"
#include "message_box.h"
#include "misc/path-conversion.h"
#include "chat/chat_manager-old.h"
#include "gui/widgets/chat_widget.h"


#include "../history/history.h"
#include "sql_history.h"

extern "C" KADU_EXPORT int sql_history_init(bool firstLoad)
{
	kdebugf();
	historySqlStorage = new HistorySqlStorage(firstLoad);
	MainConfigurationWindow::registerUiFile(dataPath("kadu/modules/configuration/sql_history.ui"));
	kdebugf2();
	return 0;
}

extern "C" KADU_EXPORT void sql_history_close()
{
	kdebugf();
	MainConfigurationWindow::unregisterUiFile(dataPath("kadu/modules/configuration/sql_history.ui"));
	QSqlDatabase::removeDatabase("kadu-history");
	kdebugf2();
}

void HistorySqlStorage::mainConfigurationWindowCreated(MainConfigurationWindow *mainConfigurationWindow)
{
/*
	portSpinBox = dynamic_cast<QSpinBox *>(widgetById("sql_history/databasehostport"));
	connect(portSpinBox, SIGNAL(valueChanged(int)), this, SLOT(portSpinBoxValueChanged(int)));

	driverComboBox = dynamic_cast<QComboBox *>(mainConfigurationWindow->widgetById("sql_history/dbdriver"));
	connect(driverComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(driverComboBoxValueChanged(int)));
	hostLineEdit = dynamic_cast<QLineEdit *>(mainConfigurationWindow->widgetById("sql_history/databasehost"));
	nameLineEdit = dynamic_cast<QLineEdit *>(mainConfigurationWindow->widgetById("sql_history/databasename"));
	userLineEdit = dynamic_cast<QLineEdit *>(mainConfigurationWindow->widgetById("sql_history/databaseuser"));
	passLineEdit = dynamic_cast<QLineEdit *>(mainConfigurationWindow->widgetById("sql_history/databasepass"));
	prefixLineEdit = dynamic_cast<QLineEdit *>(mainConfigurationWindow->widgetById("sql_history/databaseprefix"));
	if(config_file.readEntry("History", "DatabaseDriver") == "QSQLITE")
	{
		portSpinBox->setEnabled(false);
		hostLineEdit->setEnabled(false); 
		userLineEdit->setEnabled(false); 
		nameLineEdit->setEnabled(false); 
		passLineEdit->setEnabled(false);
		prefixLineEdit->setEnabled(false);
	}
*/
}

void HistorySqlStorage::configurationWindowApplied()
{

}

void HistorySqlStorage::driverComboBoxValueChanged(int index)
{
/*
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
*/
}

void HistorySqlStorage::portSpinBoxValueChanged(int value)
{
/*
	if (portSpinBox->value() < value)
		portSpinBox->setValue(value);
*/
}



HistorySqlStorage::HistorySqlStorage(bool firstLoad) : QObject()
{
	kdebugf();
	createDefaultConfiguration();

// 	connect(chat_manager, SIGNAL(messageSentAndConfirmed(ContactList, const QString&)),
// 		this, SLOT(messageSentAndConfirmed(ContactList, const QString&)));
// 	connect(chat_manager, SIGNAL(chatWidgetCreated(ChatWidget *, time_t)),
// 		this, SLOT(chatCreated(ChatWidget*)));
// 	connect(chat_manager, SIGNAL(chatWidgetDestroying(ChatWidget *)), this, SLOT(chatDestroying(ChatWidget *)));
// 	connect(kadu, SIGNAL(removingUsers(ContactList)),
// 		this, SLOT(removingUsers(ContactList)));
// 	connect(userlist, SIGNAL(statusChanged(Contact, QString, const UserStatus &, bool, bool)),
// 		this, SLOT(statusChanged(Contact, QString, const UserStatus &, bool, bool)));

/*


	foreach(ChatWidget *it, chat_manager->chats())
		chatCreated(it);*/

	initializeDatabase();
	kdebugf2();
}

HistorySqlStorage::~HistorySqlStorage()
{
	kdebugf();

// 	disconnect(kadu, SIGNAL(removingUsers(ContactList)), this, SLOT(removingUsers(ContactList)));
// 	disconnect(chat_manager, SIGNAL(messageSentAndConfirmed(ContactList,
// 						const QString&)),
// 		this, SLOT(messageSentAndConfirmed(ContactList, const QString&)));
// 	disconnect(userlist, SIGNAL(statusChanged(Contact, QString, const UserStatus &, bool, bool)), this, SLOT(statusChanged(Contact, QString, const UserStatus &, bool, bool)));
// 	disconnect(chat_manager, SIGNAL(chatWidgetCreated(ChatWidget *)), this, SLOT(chatCreated(ChatWidget *)));
// 	disconnect(chat_manager, SIGNAL(chatWidgetDestroying(ChatWidget *)), this, SLOT(chatDestroying(ChatWidget *)));
 
// 	foreach(ChatWidget* it, chat_manager->chats())
// 		chatDestroying(it);

	kdebugf2();
}

void HistorySqlStorage::messageReceived(Chat *chat, Contact contact, const QString &message)
{

}


// void HistorySqlStorage::statusChanged(Contact elem, QString protocolName, const UserStatus & oldStatus, bool /*massively*/, bool /*last*/)
// {
// 	kdebugf();
// 	//�ohoho...
// 	if (!config_file.readBoolEntry("History", "Enable") || !config_file.readBoolEntry("History", "SaveStatusChanges") || (!elem.data("history_save_status").toBool() && !config_file.readBoolEntry("History", "SaveStatusChangesForAll")) || (config_file.readBoolEntry("History", "IgnoreSomeStatusChanges") && ((elem.status(protocolName).isOnline() && oldStatus.isBusy()) || (elem.status(protocolName).isBusy() && oldStatus.isOnline()))) || (!elem.status(protocolName).hasDescription() && config_file.readBoolEntry("History", "SaveOnlyStatusWithDescription")) || elem.status(protocolName) == oldStatus && elem.status(protocolName).description() != oldStatus.description()) 
// 	{
// 		kdebugm(KDEBUG_INFO|KDEBUG_FUNCTION_END, "not appending\n");
// 		return;
// 	}
// 	appendStatus(elem, protocolName);
// 	kdebugf2();
// }

void HistorySqlStorage::appendStatus(Contact elem, QString protocolName)
{
	kdebugf();
	QHostAddress ip;
	unsigned short port;
	QString desc, query_str;
	if(debe.driverName() == "QSQLITE")
		query_str  = "INSERT INTO kadu_status (uid_group_id, status, time, description, ip_port) VALUES ('%1', '%2', datetime('now', 'localtime'), '%3', '%4');";
	else if(debe.driverName() == "QMYSQL" || debe.driverName() == "QPSQL")
		query_str  = "INSERT INTO kadu_status (uid_group_id, status, time, description, ip_port) VALUES ('%1', '%2', NOW(), '%3', '%4');";
	QString uid_group_str = findUidGroup(ContactList(elem));
	if (uid_group_str == "")
		uid_group_str = addUidGroup(ContactList(elem));
	if (uid_group_str == "")
	{
		kdebugf2();
		return;
	}
// 	if (userlist->contains(protocolName, elem.ID(protocolName), FalseForAnonymous))
// 	{
// 		Contact user = userlist->byID(protocolName, elem.ID(protocolName));
// 		ip = user.IP(protocolName);
// 		port = user.port(protocolName);
// 	}
// 	else
// 	{
		ip.setAddress((unsigned int)0);
		port = 0;
// 	}
	QString addr = ip.toString();
	if (port)
		addr = addr + QString(":") + QString::number(port);
// 	if (elem.status(protocolName).hasDescription())
// 	{
// 		desc = elem.status(protocolName).description();
// 		HtmlDocument::escapeText(desc);
// 	}

	query_str = query_str.arg(uid_group_str).arg(QString(/*elem.status(protocolName).toString().toUtf8().data()*/)).arg(QString(desc.toUtf8().data())).arg(QString(addr.toUtf8().data()));
	executeQuery(query_str);
	kdebugf2();
}

bool HistorySqlStorage::beginTransaction()
{
	kdebugf();
	if (!debe.transaction())
		return false;
	return true;
	kdebugf2();
}

bool HistorySqlStorage::commitTransaction()
{
	kdebugf();
	if (!debe.commit())
		return false;
	return true;
	kdebugf2();
}

bool HistorySqlStorage::rollbackTransaction()
{
	kdebugf();
	if (!debe.rollback())
		return false;
	return true;
	kdebugf2();
}

QString HistorySqlStorage::findUidGroup(ContactList users)
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
		query_str += ("t" + QString::number(i) + ".uid = '" + /*users[i].ID((*users[i].protocolList().begin())) +*/ "' AND t" + QString::number(i) + ".protocol = '" /*+ (*users[i].protocolList().begin())*/ + "'");
	}
	for (unsigned int i = 0; i < (unsigned int)users.count() - 1; i++)
		query_str += (" and t" + QString::number(i) + ".id = t" + QString::number(i + 1) + ".id");
	kdebug("query: %s\n", query_str.toLocal8Bit().data());
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
	QString result = query.value(0).toString();
	kdebugf2();
	return result;
}

QString HistorySqlStorage::findNewUidGroupId()
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

QString HistorySqlStorage::addUidGroup(ContactList users)
{
	kdebugf();
	QString id_str = findNewUidGroupId();
	if (id_str == "")
	{
		kdebugf2();
		return "";
	}
	kdebug("adding uid group %s\n", id_str.toLocal8Bit().data());
	for (unsigned int i = 0; i < (unsigned int)users.count(); i++)
	{
// 		QString query_str = "INSERT INTO kadu_uid_groups (id, protocol, uid) VALUES('%1', '%2', '%3')";
// 		QString uid_str = users[i].ID((*users[i].protocolList().begin()));
// 		QString proto_str = (*users[i].protocolList().begin());
// 		query_str = query_str.arg(id_str).arg(proto_str).arg(uid_str);
// 		executeQuery(query_str);
	}
	kdebugf2();
	return id_str;
}

void HistorySqlStorage::chatMsgReceived(Protocol *protocol, ContactList senders,
	const QString& msg, time_t time)
{
	kdebugf();
// 	if (!config_file.readBoolEntry("History", "Enable") || !config_file.readBoolEntry("History", "SaveChats"))
// 	{
// 		kdebugm(KDEBUG_INFO|KDEBUG_FUNCTION_END, "not appending\n");
// 		return;
// 	}
// 	foreach(const Contact &sender, senders)
// 		if ((config_file.readBoolEntry("History", "DontSaveChatsWithAnonymous") && sender.isAnonymous()) || (!sender.data("history_save_chats").toBool() && !config_file.readBoolEntry("History", "SaveChatsForAll")))
// 		{
// 			kdebugm(KDEBUG_INFO|KDEBUG_FUNCTION_END, "not appending\n");
// 			return;
// 		} 
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

void HistorySqlStorage::messageSentAndConfirmed(ContactList receivers, const QString& message, time_t time)
{
	kdebugf();
// 	if (!config_file.readBoolEntry("History", "Enable") || !config_file.readBoolEntry("History", "SaveChats"))
// 	{
// 		kdebugm(KDEBUG_INFO|KDEBUG_FUNCTION_END, "not appending\n");
// 		return;
// 	}
// 	foreach(const Contact &receiver, receivers)
// 		if ((config_file.readBoolEntry("History", "DontSaveChatsWithAnonymous") && receiver.isAnonymous()) || (!receiver.data("history_save_chats").toBool() && !config_file.readBoolEntry("History", "SaveChatsForAll")))
// 		{
// 			kdebugm(KDEBUG_INFO|KDEBUG_FUNCTION_END, "not appending\n");
// 			return;
// 		} 
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

void HistorySqlStorage::appendMessageEntry(ContactList list, const QString &msg, bool outgoing, time_t send_time, time_t receive_time)
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

void HistorySqlStorage::appendSmsEntry(ContactList list, const QString &msg, bool outgoing, time_t send_time, time_t receive_time)
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


void HistorySqlStorage::appendStatusEntry(ContactList list, const QString &status, const QString &desc, time_t time, const QString &ip_port)
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

void HistorySqlStorage::appendSms(const QString &mobile, const QString &msg)
{
	kdebugf();

	QString query_str;
	if(debe.driverName() == "QSQLITE")
		query_str = "INSERT INTO kadu_sms (uid_group_id, is_outgoing, send_time, receive_time, content) VALUES ('%1', '1', datetime('now', 'localtime'), datetime('now', 'localtime'), '%2');";
	else if(debe.driverName() == "QMYSQL" || debe.driverName() == "QPSQL")
		query_str = "INSERT INTO kadu_sms (uid_group_id, is_outgoing, send_time, receive_time, content) VALUES ('%1', '1', NOW(), NOW(), '%2');";
	Contact mob = Contact();
//	mob.addProtocol("SMS", mobile);
	ContactList mobs = ContactList(mob);
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

void HistorySqlStorage::removeHistory(const ContactList& uids, const QDate &date, HistoryEntryType type)
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
		//i posprz�taj po sobie
		query_str = "DELETE FROM kadu_uid_groups WHERE id='%1';";
		query_str = query_str.arg(uid_group_str);
		executeQuery(query_str);
	}
	kdebugf2();

}

void HistorySqlStorage::configurationUpdated()
{
	kdebugf();
	initializeDatabase();
	kdebugf2();
}


QList<ContactList> HistorySqlStorage::getAllUidGroups()
{
	kdebugf();
	return getUidGroups("SELECT id, protocol, uid FROM kadu_uid_groups;");
	kdebugf2();
}

QList<ContactList> HistorySqlStorage::getChatUidGroups()
{
	kdebugf();
	return getUidGroups("SELECT id as aj, protocol, uid FROM kadu_uid_groups WHERE (SELECT content FROM kadu_messages WHERE uid_group_id=aj LIMIT 1) != '';");
	kdebugf2();
}

QList<ContactList> HistorySqlStorage::getStatusUidGroups()
{
	kdebugf();
	return getUidGroups("SELECT id as aj, protocol, uid FROM kadu_uid_groups WHERE (SELECT status FROM kadu_status WHERE uid_group_id=aj LIMIT 1) != '';");
	kdebugf2();
}

QList<ContactList> HistorySqlStorage::getSmsUidGroups()
{
	kdebugf();
	return getUidGroups("SELECT id as aj, protocol, uid FROM kadu_uid_groups WHERE (SELECT content FROM kadu_sms WHERE uid_group_id=aj LIMIT 1) != '';");
	kdebugf2();
}

QList<ContactList> HistorySqlStorage::getUidGroups(QString queryString)
{
	kdebugf();

	QList<ContactList> result;
/*
	QSqlQuery query(debe);
	if (!query.exec(queryString))
	{
		MessageBox::msg(query.lastError().text(), false, "Warning");
		kdebugf2();
		return result;
	}
	int prev_id = -1;
	ContactList user_list_elements;
	while (query.next())
	{
		int id = query.value(0).toInt();
		QString protocol = query.value(1).toString();
		QString uid = query.value(2).toString();
		Contact user_list_element;
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
*/
	kdebugf2();
	return result;
}



QList<QDate> HistorySqlStorage::getAllDates()
{
	kdebugf();
	QList<QDate> result;
	QSqlQuery query(debe);
	QString query_str = "SELECT DISTINCT date(send_time) FROM kadu_messages";
	kdebug("query: %s\n", query_str.toLocal8Bit().data());
	if (!query.exec(query_str))
	{
		MessageBox::msg(query.lastError().text(), false, "Warning");
		kdebugf2();
		return result;
	}
	while (query.next())
	{
		QDate date = query.value(0).toDate();
		result.append(date);
	}
	kdebug("%i dates\n", result.count());
	kdebugf2();
	return result;
}


QList<QDate> HistorySqlStorage::historyDates(const ContactList& uids)
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
	kdebug("query: %s\n", query_str.toLocal8Bit().data());
	if (!query.exec(query_str))
	{
		MessageBox::msg(query.lastError().text(), false, "Warning");
		kdebugf2();
		return result;
	}
	while (query.next())
	{
		QDate date = query.value(0).toDate();
		result.append(date);
	}
	kdebug("%i dates\n", result.count());
	kdebugf2();
	return result;
}

QList<QDate> HistorySqlStorage::historyStatusDates(const ContactList& uids)
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
	kdebug("query: %s\n", query_str.toLocal8Bit().data());
	if (!query.exec(query_str))
	{
		MessageBox::msg(query.lastError().text(), false, "Warning");
		kdebugf2();
		return result;
	}
	while (query.next())
	{
		QDate date = query.value(0).toDate();
		result.append(date);
	}
	kdebug("%i dates\n", result.count());
	kdebugf2();
	return result;
}

QList<QDate> HistorySqlStorage::historySmsDates(const ContactList& uids)
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
	kdebug("query: %s\n", query_str.toLocal8Bit().data());
	if (!query.exec(query_str))
	{
		MessageBox::msg(query.lastError().text(), false, "Warning");
		kdebugf2();
		return result;
	}
	while (query.next())
	{
		QDate date = query.value(0).toDate();
		result.append(date);
	}
	kdebug("%i dates\n", result.count());
	kdebugf2();
	return result;
}

QList<ChatMessage*> HistorySqlStorage::historyMessages(const ContactList& uids, QDate date)
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
	kdebug("query: %s\n", query_str.toLocal8Bit().data());
	if (!query.exec(query_str))
	{
		MessageBox::msg(query.lastError().text(), false, "Warning");
		kdebugf2();
		return result;
	}
	while (query.next())
	{
		bool outgoing = query.value(1).toBool();
		QDateTime send_time = query.value(2).toDateTime();
		QDateTime receive_time = query.value(3).toDateTime();
		QString message = query.value(4).toString();

		ChatMessage* chat_message;

// 		if(outgoing)
// 			chat_message = new ChatMessage(kadu->myself(), uids, QString::fromUtf8(message), TypeSent, receive_time, send_time);
// 		else
// 			chat_message = new ChatMessage((*uids.begin()), ContactList(kadu->myself()), QString::fromUtf8(message), TypeReceived, receive_time, send_time);

		result.append(chat_message);
	}
	
	kdebug("%i messages\n", result.count());
	kdebugf2();
	return result;
}

QList<ChatMessage*> HistorySqlStorage::getStatusEntries(const ContactList& uids, QDate date)
{
	kdebugf();

	QList<ChatMessage*> result;
/*
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
	kdebug("query: %s\n", query_str.toLocal8Bit().data());
	if (!query.exec(query_str))
	{
		MessageBox::msg(query.lastError().text(), false, "Warning");
		kdebugf2();
		return result;
	}
	while (query.next())
	{
		QDateTime send_time = query.value(1).toDateTime();
		QDateTime receive_time = query.value(1).toDateTime();
		QString message = query.value(0).toString();
		UserStatus us = UserStatus();
		us.fromString(query.value(0).toString());
		us.setDescription(query.value(2).toString());
		QStringList address = query.value(3).toString().split(':');
		Contact user = (*uids.begin());
		if(!address.isEmpty())
			if(address.count() == 1)
				user.setAddressAndPort("Gadu", QHostAddress(address.at(0)), 0);
			else if(address.count() == 2)
				user.setAddressAndPort("Gadu", QHostAddress(address.at(0)), address.at(1).toUInt());
		user.setStatus("Gadu", us, true, false);
		ChatMessage* chat_message = new ChatMessage(user, ContactList(kadu->myself()), QString::fromUtf8(message + " : " + query.value(2).toString()), TypeReceived, receive_time, send_time);
		result.append(chat_message);
	}
	
	kdebug("%i status entries\n", result.count());
*/
	kdebugf2();
	return result;
}

QList<ChatMessage*> HistorySqlStorage::getSmsEntries(const ContactList& uids, QDate date)
{
	kdebugf();
	QList<ChatMessage*> result;
/*
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

	kdebug("query: %s\n", query_str.toLocal8Bit().data());
	if (!query.exec(query_str))
	{
		MessageBox::msg(query.lastError().text(), false, "Warning");
		kdebugf2();
		return result;
	}
	while (query.next())
	{
		QDateTime send_time = query.value(0).toDateTime();
		QString message = query.value(1).toString();
		ChatMessage* chat_message;
			chat_message = new ChatMessage(kadu->myself(), uids, QString::fromUtf8(message), TypeSent, send_time, send_time);

		result.append(chat_message);
	}
	
	kdebug("%i status entries\n", result.count());
*/
	kdebugf2();
	return result;
}


int HistorySqlStorage::getEntriesCount(const QList<ContactList> &uids, HistoryEntryType type)
{
	kdebugf();
	int result = 0;
	QSqlQuery query(debe);
	QString query_str;

	foreach(const ContactList uid, uids)
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
	kdebug("query: %s\n", query_str.toLocal8Bit().data());
	if (!query.exec(query_str))
	{
		MessageBox::msg(query.lastError().text(), false, "Warning");
		kdebugf2();
		return 0;
	}
	query.next();
	int part = query.value(0).toInt();
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
	kdebug("query: %s\n", query_str.toLocal8Bit().data());
	if (!query.exec(query_str))
	{
		MessageBox::msg(query.lastError().text(), false, "Warning");
		kdebugf2();
		return 0;
	}
	query.next();
	int part = query.value(0).toInt();
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
	kdebug("query: %s\n", query_str.toLocal8Bit().data());
	if (!query.exec(query_str))
	{
		MessageBox::msg(query.lastError().text(), false, "Warning");
		kdebugf2();
		return 0;
	}
	query.next();
	int part = query.value(0).toInt();
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
	kdebug("query: %s\n", query_str.toLocal8Bit().data());
	if (!query.exec(query_str))
		{
			MessageBox::msg(query.lastError().text(), false, "Warning");
			kdebugf2();
			return 0;
		}
		query.next();
		int sub = query.value(0).toInt();
		part += sub;

			query_str = "SELECT COUNT(uid_group_id) FROM kadu_status WHERE uid_group_id = '%1';";
			uid_group_str = findUidGroup(uid);
	if (uid_group_str == "")
	{
		kdebugf2();
		return 0;
	}
	query_str = query_str.arg(uid_group_str);
	kdebug("query: %s\n", query_str.toLocal8Bit().data());
	if (!query.exec(query_str))
		{
			MessageBox::msg(query.lastError().text(), false, "Warning");
			kdebugf2();
			return 0;
		}
		query.next();
		sub = query.value(0).toInt();
		part += sub;

			query_str = "SELECT COUNT(uid_group_id) FROM kadu_sms WHERE uid_group_id = '%1';";
	uid_group_str = findUidGroup(uid);
	if (uid_group_str == "")
	{
		kdebugf2();
		return 0;
	}
	query_str = query_str.arg(uid_group_str);
	kdebug("query: %s\n", query_str.toLocal8Bit().data());
	if (!query.exec(query_str))
		{
			MessageBox::msg(query.lastError().text(), false, "Warning");
			kdebugf2();
			return 0;
		}
		sub = query.value(0).toInt();
		part += sub;
		result +=part;
	}	
}
kdebugf2();
return result;
	
}

void HistorySqlStorage::createDefaultConfiguration()
{
// 	config_file.addVariable("History", "DatabaseDriver", "QSQLITE");
// 	config_file.addVariable("History", "DatabaseFilePath", ggPath("/history/history.db"));
// 	config_file.addVariable("History", "DatabaseTableNamePrefix", "kadu_");

//TODO: by doda� reszt� warto�ci, je�li jezzcze czego� brak

}

void HistorySqlStorage::initializeDatabase()
{
	kdebugf();
	tableNamePrefix = "";//config_file.readEntry("History", "DatabaseTableNamePrefix");
	QString driver = "QSQLITE";//config_file.readEntry("History", "DatabaseDriver");
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
	//if(driver == "QSQLITE")
	//	debe.setDatabaseName(config_file.readEntry("History", "DatabaseFilePath"));
// 	else if(driver == "QMYSQL")
// 	{
// 		debe.setDatabaseName(config_file.readEntry("History", "DatabaseName"));	
// 		debe.setPort(config_file.readUnsignedNumEntry("History", "DatabaseHostPort"));
// 		debe.setHostName(config_file.readEntry("History", "DatabaseHost"));
// 		debe.setUserName(config_file.readEntry("History", "DatabaseUser"));
// 		debe.setPassword(pwHash(config_file.readEntry("History", "DatabasePassword")));
// 	}
//	else if(driver == "QPSQL" || driver == "QMYSQL")
//	{
// 		debe.setDatabaseName(config_file.readEntry("History", "DatabaseName"));	
// 		debe.setPort(config_file.readUnsignedNumEntry("History", "DatabaseHostPort"));
// 		debe.setHostName(config_file.readEntry("History", "DatabaseHost"));
// 		debe.setUserName(config_file.readEntry("History", "DatabaseUser"));
// 		debe.setPassword(pwHash(config_file.readEntry("History", "DatabasePassword")));
//	}
	

	if(debe.open())
		kdebug("Connected to database, driver: %s\n", driver.toLocal8Bit().data());
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

HistorySearchResult HistorySqlStorage::searchHistory(ContactList users, HistorySearchParameters params)
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
		//mo�na by szuka� i po statusie, ale jaki� problem z 'OR ... ' w zapytaniu - brak wynik�w wtedy
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
	kdebug("query: %s\n", query.executedQuery().toLocal8Bit().data());
/*
	while(query.next())
	{	
		title = QString::fromUtf8(query.value(1).toString());
		title.truncate(40);
		title += " ...";
		QDate recvDate = query.value(0).toDate();
		HistorySearchDetailsItem d = HistorySearchDetailsItem((*users.begin()).altNick(), title, recvDate, 1);
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
*/
	result.pattern = params.pattern;
	return result;
	kdebugf2();
}

QString HistorySqlStorage::prepareText(const QString &text)
{
	QString str = text;
	str.replace('\'', "\'\'");
	str.replace('\\', "\\\\");
	return str;
}

void HistorySqlStorage::executeQuery(const QString &query)
{
	kdebugf();
	QSqlQuery sqlQuery(debe);
	if (!sqlQuery.exec(query))
	{
		MessageBox::msg(sqlQuery.lastError().text(), false, "Warning");
		return;
	} 
	kdebug("query: %s\n", query.toLocal8Bit().data());
	kdebugf2();
}

HistorySqlStorage *historySqlStorage = 0;
