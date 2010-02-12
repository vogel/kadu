/*
 * %kadu copyright begin%
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2008, 2009, 2010 RafaÅ‚ Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2008 MichaÅ‚ Podsiadlik (michal@kadu.net)
 * Copyright 2008 Tomasz RostaÅ„ski (rozteck@interia.pl)
 * Copyright 2008 Piotr Galiszewski (piotrgaliszewski@gmail.com)
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

/**

Copyright (C) 2005 by 
	Pan Wojtas (Wojtek Sulewski)
	wojciech <  _at_    > sulewski.pl
	gg: 2087202

Na podstawie skryptu TCL autorstwoa Attis'a.
Czê¶æ kodu (atak flood i emotikonami) na podstawie ³aty amd_fanatyka
Przystosowanie do kadu 0.6 -  White Eagle
Nowa funkcjonalnosc - Dorregaray
(szczegoly w zalaczonym pliku Changelog)

**/

#include "firewall.h"

#include <QtCore/QTimer>	//singleShot
#include <QtCore/QStringList>
#include <QtGui/QLabel>
#include <QtGui/QTextEdit>
#include <QtGui/QGridLayout>
#include <QtGui/QMessageBox>
#include <QtGui/QToolTip>
#include <QtGui/QListWidget>
#include <QtGui/QListWidgetItem>

#include "../notify/notify.h"
#include "../history/history.h"

#include "accounts/account.h"
#include "accounts/account_manager.h"
#include "config_file.h"
#include "icons-manager.h"
#include "misc/misc.h"
#include "search.h"
#include "kadu.h"
#include "chat_manager-old.h"
#include "usergroup.h"
#include "debug.h"


Firewall *firewall;

extern "C" KADU_EXPORT int firewall_init()
{
	firewall = new Firewall();
	MainConfigurationWindow::registerUiFile(dataPath("kadu/modules/configuration/firewall.ui"), firewall);
	notification_manager->registerEvent("Firewall", "Firewall notifications", CallbackNotRequired);

	return 0;
}

extern "C" KADU_EXPORT void firewall_close()
{
	notification_manager->unregisterEvent("Firewall");
	MainConfigurationWindow::unregisterUiFile(dataPath("kadu/modules/configuration/firewall.ui"), firewall);	
	delete firewall;
	firewall = 0;
}


Firewall::Firewall() : flood_messages(0), right_after_connection(false)
{
	kdebugf();
	
	loadSecuredList();

	lastMsg.start();
	lastNotify.start();

	pattern.setCaseSensitive(false);
	pattern.setPattern(unicode2std(config_file.readEntry("Firewall", "answer", tr("I want something"))));

	Protocol *gadu = AccountManager::instance()->defaultAccount()->protocol();
	connect(gadu, SIGNAL(rawGaduReceivedMessageFilter(Protocol *, UserListElements, QString&, QByteArray&, bool&)), this, SLOT(messageFiltering(Protocol *, UserListElements, QString&, QByteArray&, bool&)));
	connect(gadu, SIGNAL(sendMessageFiltering(const UserListElements, QByteArray &, bool &)), this, SLOT(sendMessageFilter(const UserListElements, QByteArray &, bool &)));
 	connect(chat_manager, SIGNAL(chatWidgetDestroying(ChatWidget *)), this, SLOT(chatDestroyed(ChatWidget *)));

	connect(userlist, SIGNAL(userDataChanged(UserListElement, QString, QVariant, QVariant, bool, bool)), this, SLOT(userDataChanged(UserListElement, QString, QVariant, QVariant, bool, bool)));
	connect(userlist, SIGNAL(userAdded(UserListElement, bool, bool)), this, SLOT(userAdded(UserListElement, bool, bool)));
	connect(userlist, SIGNAL(userRemoved(UserListElement, bool, bool)), this, SLOT(userRemoved(UserListElement, bool, bool)));

	
	connect(gadu, SIGNAL(connecting()), this, SLOT(connecting()));
	connect(gadu, SIGNAL(connected()), this, SLOT(connected()));

	defaultSettings();
	
	kdebugf2();
	
}

Firewall::~Firewall()
{
	kdebugf();

	Protocol *gadu = AccountManager::instance()->defaultAccount()->protocol();	
	disconnect(gadu, SIGNAL(rawGaduReceivedMessageFilter(Protocol *, UserListElements, QString&, QByteArray&, bool&)), this, SLOT(messageFiltering(Protocol *, UserListElements, QString&, QByteArray&, bool&)));
	disconnect(gadu, SIGNAL(sendMessageFiltering(const UserListElements, QByteArray &, bool &)), this, SLOT(sendMessageFilter(const UserListElements, QByteArray &, bool &)));
	disconnect(chat_manager, SIGNAL(chatWidgetDestroying(ChatWidget *)), this, SLOT(chatDestroyed(ChatWidget *)));

	disconnect(userlist, SIGNAL(userDataChanged(UserListElement, QString, QVariant, QVariant, bool, bool)), this, SLOT(userDataChanged(UserListElement, QString, QVariant, QVariant, bool, bool)));
	disconnect(userlist, SIGNAL(userAdded(UserListElement, bool, bool)), this, SLOT(userAdded(UserListElement, bool, bool)));
	disconnect(userlist, SIGNAL(userRemoved(UserListElement, bool, bool)), this, SLOT(userRemoved(UserListElement, bool, bool)));
	
	disconnect(gadu, SIGNAL(connecting()), this, SLOT(connecting()));
	disconnect(gadu, SIGNAL(connected()), this, SLOT(connected()));
	
	
	kdebugf2();
	
}

void Firewall::messageFiltering(Protocol *protocol, UserListElements senders, QString& msg, QByteArray& formats, bool& stop)
{
	kdebugf();

	QString user;
	// czy senders.count() mo¿e byæ < 1 ???
	if (senders.count() > 1)
		user = tr("conference") + " " + senders[0].ID("Gadu");
	else if (userlist->contains(senders[0], FalseForAnonymous))
		user = senders[0].altNick();
	else
		user = senders[0].ID("Gadu");
	
// emotikony s± sprawdzane nawet przy ³±czeniu
	const int min_interval_notify = 2000;
	
	if (config_file.readBoolEntry("Firewall", "dos_emoticons", true))
	{
		bool ignore = false;
		
		if (config_file.readBoolEntry("Firewall", "emoticons_allow_known", false))
		{
			ignore = true;
			foreach(const UserListElement &user, senders)
			{
				if (!userlist->contains(user, FalseForAnonymous))
				{
					ignore = false;
					break;
				}
			}
		}

		if (!ignore && checkEmoticons(msg))
		{
			stop = true;
			if (lastNotify.elapsed() > min_interval_notify)
			{
				showHint(user, tr("flooding DoS attack with emoticons!"));
				if (config_file.readBoolEntry("Firewall", "write_log", true))
				{
					writeLog(user, msg);
				}
	
				lastNotify.restart();
			}
			kdebugf2();
			return;
		}
	}
	

// atak floodem
	if (config_file.readBoolEntry("Firewall", "dos", true) &&
			checkFlood())
	{
		stop = true;
		if (lastNotify.elapsed() > min_interval_notify)
		{
			showHint(user, tr("flooding DoS attack!"));
			if (config_file.readBoolEntry("Firewall", "write_log", true))
				writeLog(user, msg);
			
			lastNotify.restart();
		}
		kdebugf2();
		return;
	}

// ochrona przed anonimami
	if (config_file.readBoolEntry("Firewall", "chats", true) &&
				checkChat(protocol, unicode2std(msg), senders, user, stop))
	{
		stop = true;
	}

// ochrona przed konferencjami
	if (config_file.readBoolEntry("Firewall", "ignore_conferences", true) &&
				checkConference(msg, senders, user))
	{
		stop = true;
	}

// wiadomosc zatrzymana. zapisz do loga i wyswietl dymek
	if (stop)
	{
		if (msg.length() > 50)
			showHint(user, msg.left(50).append("..."));
		else
			showHint(user, msg);

		if (config_file.readBoolEntry("Firewall", "write_log", true))
			writeLog(user, msg);
		if (config_file.readBoolEntry("Firewall", "write_history", true)) {
			time_t curr_time = QDateTime::currentDateTime(Qt::LocalTime).toTime_t();
			history->appendMessage(UinsList(user.toUInt()), user.toUInt(), msg,
			    false, curr_time, false, curr_time);
		}
	}

	kdebugf2();
}

bool Firewall::checkConference(const QString &message, const UserListElements senders, const QString &user)
{
	kdebugf();

	// TODO: chyba niepotrzebne
	if (senders.count() <= 1)
		return false;

	foreach(const UserListElement &user, senders)
	{
 		if (userlist->contains(user, FalseForAnonymous) || passed.contains(user))
		{
			kdebugf2();
 			return false;
		}
	}
	
	kdebugf2();
	return true;
}

bool Firewall::checkChat(Protocol *protocol, const QString &message, const UserListElements senders, const QString &user, bool &stop)
{
	kdebugf();

	// konferencja
 	if (senders.count() > 1)
	{
		kdebugf2();
 		return false;
	}

 	if (userlist->contains(senders[0], FalseForAnonymous) || passed.contains(senders[0]))
	{
		kdebugf2();
 		return false;
	}

	Protocol *gadu = AccountManager::instance()->defaultAccount()->protocol();
	if (gadu->currentStatus().isInvisible() && config_file.readBoolEntry("Firewall", "drop_anonymous_when_invisible", false))
	{
		if (config_file.readBoolEntry("Firewall", "write_log", true))
				writeLog(user, tr("Chat with anonim silently dropped.\n") + "----------------------------------------------------\n");
		kdebugf2();
 		return true;
	}

	/*if (config_file.readBoolEntry("Firewall", "ignore_invisible", false))
	{
		UserStatus status = senders[0].status("Gadu");
		kdebugm(KDEBUG_INFO, "\n\n\n\n\n\nStatus: %s\n", status.toString().local8Bit().data());
		if (status.isOffline() || status.isInvisible()) {
			if (!right_after_connection)
				gadu->sendMessage(senders, tr("This message has been generated AUTOMATICALLY!\n\nI'm a busy person and I don't have time for stupid chats with the persons hiding itselfs. If you want to talk with me change the status to Online or Busy first."));

			if (config_file.readBoolEntry("Firewall", "write_log", true))
				writeLog(user, tr("Chat with invisible anonim ignored.\n") + "----------------------------------------------------\n");
			
			kdebugf2();
			return true;
		}
	}*/

	if (pattern.exactMatch(message.stripWhiteSpace()))
	{
		passed.append(senders[0]);

		if (config_file.readBoolEntry("Firewall", "confirmation", true))
			protocol->sendMessage(senders, config_file.readEntry("Firewall", "confirmation_text", tr("OK, now say hello, and introduce yourself ;-)")));
		
		if (config_file.readBoolEntry("Firewall", "write_log", true))
			writeLog(user, tr("User wrote right answer!\n") + "----------------------------------------------------\n");
		
		stop = true;
		
		kdebugf2();
		return false;
	}
	else
	{
		if (last_uin != user && config_file.readBoolEntry("Firewall", "search", true))
		{
			SearchDialog *sd;
			sd = new SearchDialog(kadu, user.toUInt());
			sd->show();
			sd->firstSearch();
			
			last_uin = user;
		}
		
		kdebugm(KDEBUG_INFO, "%s\n", QString(message).local8Bit().data());

		if (!right_after_connection)
			protocol->sendMessage(senders, config_file_ptr->readEntry("Firewall", "question", tr("This message has been generated AUTOMATICALLY!\n\nI'm a busy person and I don't have time for stupid chats. Find another person to chat with. If you REALLY want something from me, simple type \"I want something\" (capital doesn't matter)")));
		
		kdebugf2();
		return true;
	}
}

bool Firewall::checkFlood()
{
	kdebugf();
	
	const int min_interval = config_file.readNumEntry("Firewall", "dos_interval", 500); // minimalna przerwa czasowa miedzy przeslaniem 2 kolejnych wiadomosci
	const unsigned int max_flood_messages = 15;
		
	if (lastMsg.restart() >= min_interval)
	{
		flood_messages = 0;
		kdebugf2();
		return false;
	}

	if (flood_messages < max_flood_messages)
	{
		flood_messages++;
		kdebugf2();
		return false;
	}

	kdebugf2();
	return true;
}

bool Firewall::checkEmoticons(const QString &message)
{
	kdebugf();
	
	const unsigned int max_occ = config_file.readNumEntry("Firewall", "emoticons_max", 15); // maksymalna laczna ilosc wystapien emotikon

	QStringList emots;
	emots << "<" << ":)" << ":(" << ":-(" << ";)" << ":[" << "wrrrr!" << "niee" << "tiaaa" << ":-)" << ";-)" << ":P" << ":-P" << ";P" << ";-P" << "!!" << "??" << ";(" << ";-(" << ":D" << ":-D" << ";D" << ";-D" << ":O" << ":-O" << ";O" << ";-O" << ":>" << ";>" << ":->" << ";->" << ":|" << ";|" << ":-|" << ";-|" << ":]" << ";]" << ":-]" << ";-]" << ":/" << ";/" << ":-/" << ";-/" << ":*" << ":-*" << ";*" << ";-*" << "]:->";
		
		
	unsigned int occ = 0;
	foreach(const QString emot, emots)
	{
		occ += message.count(emot);
	}


	kdebugf2();
	return (occ > max_occ);
}

void Firewall::userDataChanged(UserListElement elem, QString name, QVariant oldValue, QVariant currentValue, bool massively, bool last)
{
	if(name == "Anonymous" && currentValue.toBool() == false)
	{
		secured.append(elem.ID("Gadu"));
		saveSecuredList();
	}
}

void Firewall::userAdded(UserListElement elem, bool mas, bool last)
{
	if(!elem.isAnonymous())
	{
		secured.append(elem.ID("Gadu"));
		saveSecuredList();
	}
}

void Firewall::userRemoved(UserListElement elem, bool mas, bool last)
{
	if (secured.contains(elem.ID("Gadu")))
	{
		secured.remove(elem.ID("Gadu"));
		saveSecuredList();
	}

}

void Firewall::connecting()
{
	kdebugf();
	
	right_after_connection = true;

	kdebugf2();
}

void Firewall::connected()
{
	kdebugf();
	
	QTimer::singleShot(4000, this, SLOT(changeRight_after_connection()));

	kdebugf2();
}

void Firewall::changeRight_after_connection()
{
	kdebugf();
	
	right_after_connection = !right_after_connection;

	kdebugf2();
}

void Firewall::chatDestroyed(ChatWidget *chat)
{
	kdebugf();
	const UserGroup group = chat->users();
//	CONST_FOREACH (i, *group)
	UserListElements users = group->toUserListElements();
	foreach(const UserListElement &user, users)
	{
		if (secured_temp_allowed.contains(user.ID("Gadu")))
			secured_temp_allowed.remove(user.ID("Gadu"));
	}

	kdebugf2();
}

void Firewall::sendMessageFilter(const UserListElements users, QByteArray/*String*/ &msg, bool &stop)
{
	kdebugf();
	
	foreach(const UserListElement &user, users)
	{
		if (!userlist->contains(user, FalseForAnonymous) && chat_manager->findChatWidget(user))
			passed.append(user);
	}
	
	if (config_file.readBoolEntry("Firewall", "safe_sending", false))
	{
		foreach(const UserListElement &user, users)
		{
			if (secured.contains(user.ID("Gadu")) && !secured_temp_allowed.contains(user.ID("Gadu")))
			{
				switch (QMessageBox::warning(0, "Kadu", tr("Are you sure you want to send this message?"), tr("&Yes"), tr("Yes and allow until chat closed"), tr("&No"), 2, 2))
				{
						default:
							stop = true;
							return;
						case 0:
							return;
						case 1:
							secured_temp_allowed.append(user.ID("Gadu"));
							return;
				}
				
			}
		}
	}
	
	
	kdebugf2();
}

void Firewall::showHint(const QString &u, const QString &m)
{
	kdebugf();

	if (config_file.readBoolEntry("Firewall", "notify", true))
	{
		UserListElement user = userlist->byID("Gadu", u);
		Notification *notification = new Notification("Firewall", "ManageKeysWindowIcon", user);
		notification->setText(config_file.readEntry("Firewall", "notification_syntax", 
			tr("%u writes")).replace("%u", u).replace("%m", ""));
		notification->setDetails(m);
		notification_manager->notify(notification);
	}
	kdebugf2();
}

void Firewall::writeLog(const QString &u, const QString &m)
{
	kdebugf();
	
	QFile log_file(config_file.readEntry("Firewall", "log_file", ggPath("firewall.log")));
				
	if (!log_file.exists())
	{
		log_file.open(IO_WriteOnly);
		QTextStream stream( &log_file );
		stream << tr("      DATA AND TIME      ::   UIN   :: MESSAGE\n") << "----------------------------------------------------\n";
		log_file.close();
	}
				
	log_file.open(IO_WriteOnly | IO_Append);
	QTextStream stream( &log_file );
	stream << QDateTime::currentDateTime(Qt::LocalTime).toString() << " :: " << u << " :: " << m << "\n";
	log_file.close();
	
	kdebugf2();
}

void Firewall::loadSecuredList()
{
	kdebugf();
	
	QString loaded_str = config_file.readEntry("Firewall", "secured_list");
	secured = QStringList::split(",", loaded_str);
	
	kdebugf2();
}

void Firewall::saveSecuredList()
{
	kdebugf();
	
	QStringList list_to_save = secured;

	config_file.writeEntry("Firewall", "secured_list", list_to_save.join(","));
	config_file.sync();
	
	kdebugf2();
}

bool Firewall::isSecured(const QString &id)
{
	return secured.findIndex(id) != -1;		// findIndex zwraca -1, je¶li nie znalaz³...
}

void Firewall::_Left(QListWidgetItem *item)
{
	kdebugf();

	allList->addItem(new QListWidgetItem(*item));
	secureList->removeItemWidget(item);
	delete(item);

	kdebugf2();
}

void Firewall::_AllLeft()
{
	int i;
	int count = secureList->count();
	QListWidgetItem *item;
	for (i = 0; i < count; ++i)
	{
		item = secureList->item(i);
		allList->addItem(new QListWidgetItem(*item));
		secureList->removeItemWidget(item);
		delete(item);
	}
}

void Firewall::_Right(QListWidgetItem *item)
{
	kdebugf();

	secureList->addItem(new QListWidgetItem(*item));
	allList->removeItemWidget(item);
	delete(item);

	kdebugf2();
}

void Firewall::_AllRight()
{
	int i;
	int count = allList->count();
	QListWidgetItem *item;
	for (i = 0; i < count; ++i)
	{
		item = allList->item(i);
		secureList->addItem(new QListWidgetItem(*item));
		allList->removeItemWidget(item);
		delete(item);
	}
}


void Firewall::mainConfigurationWindowCreated(MainConfigurationWindow *mainConfigurationWindow)
{
/*
Safe sending GUI
*/
	ConfigGroupBox *secureGroupBox = mainConfigurationWindow->configGroupBox("Firewall", "Safe sending", "Safe sending");

	QWidget *secure = new QWidget(secureGroupBox->widget());
	QGridLayout *secureLayout = new QGridLayout(secure);
	secureLayout->setSpacing(5);
	secureLayout->setMargin(5);

	allList = new QListWidget(secure);
	QPushButton *moveToSecureList = new QPushButton(tr("Move to 'Secured'"), secure);

	secureLayout->addWidget(new QLabel(tr("All"), secure), 0, 0);
	secureLayout->addWidget(allList, 1, 0);
	secureLayout->addWidget(moveToSecureList, 2, 0);

	secureList = new QListWidget(secure);
	QPushButton *moveToAllList = new QPushButton(tr("Move to 'All'"), secure);

	secureLayout->addWidget(new QLabel(tr("Secured"), secure), 0, 1);
	secureLayout->addWidget(secureList, 1, 1);
	secureLayout->addWidget(moveToAllList, 2, 1);

	connect(moveToSecureList, SIGNAL(clicked()), this, SLOT(_AllRight()));
	connect(moveToAllList, SIGNAL(clicked()), this, SLOT(_AllLeft()));
	connect(secureList, SIGNAL(itemDoubleClicked(QListWidgetItem *)), this, SLOT(_Left(QListWidgetItem *)));
	connect(allList, SIGNAL(itemDoubleClicked(QListWidgetItem *)), this, SLOT(_Right(QListWidgetItem *)));

	secureGroupBox->addWidgets(0, secure);
	
	foreach(const UserListElement &user, *userlist)
	{
		if (user.usesProtocol("Gadu") && !user.isAnonymous())
		{
			if (isSecured(user.ID("Gadu")))
				secureList->addItem(user.altNick());
			else
				allList->addItem(user.altNick());
		}
	}

	allList->sortItems();
	secureList->sortItems();

	allList->setSelectionMode(QAbstractItemView::ExtendedSelection);
	secureList->setSelectionMode(QAbstractItemView::ExtendedSelection);
/*
Automatic question GUI
*/
	ConfigGroupBox *questionGroupBox = mainConfigurationWindow->configGroupBox("Firewall", "Unknown chats protection", "Automatic question");

	QWidget *question = new QWidget(questionGroupBox->widget());
	QGridLayout *questionLayout = new QGridLayout(question);
	questionLayout->setSpacing(5);
	questionLayout->setMargin(5);
	questionLayout->addWidget(new QLabel(tr("Message:"), question), 0, 0);

	questionEdit = new QTextEdit(question);
	questionEdit->setTextFormat(Qt::PlainText);
	questionLayout->addMultiCellWidget(questionEdit, 1, 1, 0, 1);
	questionEdit->setText(config_file.readEntry("Firewall", "question"));
        QToolTip::add(questionEdit, tr("This message will be send to unknown person."));

	answerEdit = new QLineEdit(question);
	answerEdit->setText(config_file.readEntry("Firewall", "answer"));
        QToolTip::add(answerEdit, tr("Right answer for question above - you can use regexp."));
	QLabel *label = new QLabel(tr("Answer:"), question);
        QToolTip::add(label, tr("Right answer for question above - you can use regexp."));

	questionLayout->addWidget(label, 2, 0);
	questionLayout->addWidget(answerEdit, 2, 1);

	questionGroupBox->addWidgets(0, question);
/*
End creating Gui
*/
	connect(mainConfigurationWindow->widgetById("firewall/write_log"), SIGNAL(toggled(bool)),
		mainConfigurationWindow->widgetById("firewall/log_file"), SLOT(setEnabled(bool)));

	connect(mainConfigurationWindow->widgetById("firewall/notify"), SIGNAL(toggled(bool)),
		mainConfigurationWindow->widgetById("firewall/notify_syntax"), SLOT(setEnabled(bool)));

	connect(mainConfigurationWindow->widgetById("firewall/chats"), SIGNAL(toggled(bool)),
		mainConfigurationWindow->configGroupBox("Firewall", "Unknown chats protection", "Automatic question")->widget(), SLOT(setEnabled(bool)));
	connect(mainConfigurationWindow->widgetById("firewall/chats"), SIGNAL(toggled(bool)),
		mainConfigurationWindow->configGroupBox("Firewall", "Unknown chats protection", "After right answer")->widget(), SLOT(setEnabled(bool)));

	connect(mainConfigurationWindow->widgetById("firewall/confirmation"), SIGNAL(toggled(bool)),
		mainConfigurationWindow->widgetById("firewall/confirmation_text"), SLOT(setEnabled(bool)));

	connect(mainConfigurationWindow->widgetById("firewall/dos"), SIGNAL(toggled(bool)),
		mainConfigurationWindow->widgetById("firewall/dos_interval"), SLOT(setEnabled(bool)));

	connect(mainConfigurationWindow->widgetById("firewall/dos_emoticons"), SIGNAL(toggled(bool)),
		mainConfigurationWindow->widgetById("firewall/emoticons_max"), SLOT(setEnabled(bool)));
	connect(mainConfigurationWindow->widgetById("firewall/dos_emoticons"), SIGNAL(toggled(bool)),
		mainConfigurationWindow->widgetById("firewall/emoticons_allow_known"), SLOT(setEnabled(bool)));

	QWidget *safeSending = mainConfigurationWindow->widgetById("firewall/safe_sending");
	connect(safeSending, SIGNAL(toggled(bool)),
		allList, SLOT(setEnabled(bool)));
	connect(safeSending, SIGNAL(toggled(bool)),
		secureList, SLOT(setEnabled(bool)));
	connect(safeSending, SIGNAL(toggled(bool)),
		moveToSecureList, SLOT(setEnabled(bool)));
	connect(safeSending, SIGNAL(toggled(bool)),
		moveToAllList, SLOT(setEnabled(bool)));

	connect(mainConfigurationWindow, SIGNAL(configurationWindowApplied()), this, SLOT(configurationApplied()));
}

void Firewall::configurationUpdated()
{
	pattern.setPattern(unicode2std(config_file.readEntry("Firewall", "answer", tr("I want something"))));
}

void Firewall::configurationApplied()
{
	secured.clear();

	for (unsigned int i = 0; i < secureList->count(); ++i)
	{
		secured.append(userlist->byAltNick(secureList->item(i)->text()).ID("Gadu"));
	}
		
	saveSecuredList();

	config_file.writeEntry("Firewall", "question", questionEdit->text());
	config_file.writeEntry("Firewall", "answer", answerEdit->text());
}

void Firewall::defaultSettings()
{

	//domy¶lne powiadamianie dymkiem
	config_file.addVariable("Notify", "Firewall_Hints", config_file.readEntry("Firewall", "show_hint", "true"));
	config_file.addVariable("Firewall", "notification_syntax", config_file.readEntry("Firewall", "hint_syntax", tr("%u writes")));
	//domy¶lne kolory dymków
	config_file.addVariable("Hints", "Event_Firewall_fgcolor", config_file.readEntry("Firewall", "fg_color", "#000080"));//navy
	config_file.addVariable("Hints", "Event_Firewall_bgcolor", config_file.readEntry("Firewall", "bg_color", "#add8e6"));//lightblue
	//domy¶lne warto¶ci zmiennych konfiguracyjnych
	config_file.addVariable("Firewall", "ignore_conferences", true);
	config_file.addVariable("Firewall", "search", true);
	config_file.addVariable("Firewall", "chats", true);
	config_file.addVariable("Firewall", "question", tr("This message has been generated AUTOMATICALLY!\n\nI'm a busy person and I don't have time for stupid chats. Find another person to chat with. If you REALLY want something from me, simple type \"I want something\" (capital doesn't matter)") );
	config_file.addVariable("Firewall", "answer", tr("I want something") );
	config_file.addVariable("Firewall", "confirmation", true );
	config_file.addVariable("Firewall", "confirmation_text", tr("OK, now say hello, and introduce yourself ;-)") );
	config_file.addVariable("Firewall", "dos", true);
	config_file.addVariable("Firewall", "dos_interval", 500);
	config_file.addVariable("Firewall", "dos_emoticons", true);
	config_file.addVariable("Firewall", "emoticons_max", 15);
	config_file.addVariable("Firewall", "emoticons_allow_known", false);
	config_file.addVariable("Firewall", "safe_sending", false);
	config_file.addVariable("Firewall", "write_log", true);
	config_file.addVariable("Firewall", "log_file", ggPath("firewall.log"));
	config_file.addVariable("Firewall", "notify", true);
}
