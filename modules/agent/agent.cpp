/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

/*
	Author: Micha³ ¯ar³ok <mishaaq*delete_this*@gmail.com>
	Version: 0.4.3

	Based on Kadu plugin "spy" by Przemys³aw Sowa and Wojtek Sulewski
*/

#include "agent.h"
#include "agent_notifications.h"

#include <QtCore/QFile>

#include "debug.h"
#include "chat_manager.h"
#include "config_file.h"
#include "message_box.h"
#include "action.h"
#include "kadu.h"
#include "misc.h"
#include "search.h"
#include "gadu.h"
#include "icons_manager.h"
#include "modules/notify/notify.h"

#include "exports.h"


extern "C" KADU_EXPORT int agent_init()
{
	agent = new Agent();
	return 0;
}

extern "C" KADU_EXPORT void agent_close()
{
	delete agent;
	agent = NULL;
}

Agent::Agent() : QObject(NULL, "agent")
{
	kdebugf();

	connect(gadu, SIGNAL(userStatusChangeIgnored(UinType)), this, SLOT(userFound(UinType)));

	// Main menu entry
	agentActionDescription = new ActionDescription(
		ActionDescription::TypeMainMenu, "agentAction",
		this, SLOT(resultsRequest()),
		"Agent", tr("Who has me on list")
	);
	kadu->insertMenuActionDescription(0, agentActionDescription);

	if(config_file.readBoolEntry("Agent", "FirstTime", true))
	{
		QFile listFile;
		listFile.setName(QString(ggPath("spy-unknownslist").ascii()));
		if(listFile.open(IO_ReadOnly))
		{
			if(MessageBox::ask(tr("Agent has founded spy's unknown-users list. Do you want to append this list to agent module?")))
			{
				QTextStream stream(&listFile);

				QString uin_str, date_str, line;
				bool ok;
				while (!stream.atEnd())
				{
					UnknownUser user;
					bool isAlready = false;

					line = stream.readLine();
					uin_str = line.section(',', 0, 0);
					date_str = line.section(',', 1, 1);

					unsigned int uin_int = uin_str.toUInt(&ok, 10);
					if(!ok)
						kdebugm(KDEBUG_PANIC, "Couldn't cast QString to int");

					foreach(UnknownUser user, UnknownsList)
					{
						if (user.uin == uin_int)
						{
							isAlready = true;
							break;
						}
					}
					if (!isAlready)
					{
						user.uin = uin_int;
						user.date = QDate::fromString(date_str, Qt::ISODate);
						user.seq = 0;
						UnknownsList.append(user);
					}
				}
			}
			listFile.close();
		}
		config_file.writeEntry("Agent", "FirstTime", false);
	}
	loadListFromFile();

	notification_manager->registerEvent("Agent/NewFound", QT_TRANSLATE_NOOP("@default", "Agent - new user found"), CallbackRequired);

	kdebugf2();
}

Agent::~Agent()
{
	kdebugf();

	notification_manager->unregisterEvent("Agent/NewFound");

	saveListToFile();

	disconnect(gadu, SIGNAL(userStatusChangeIgnored(UinType)), this, SLOT(userFound(UinType)));

	kadu->removeMenuActionDescription(agentActionDescription);
	delete agentActionDescription;

	if(agentWidget != NULL)
	{
		delete agentWidget;
		agentWidget = NULL;
	}

	kdebugf2();
}

void Agent::userFound(UinType uin)
{
	kdebugf();

	foreach(UnknownUser user, UnknownsList)
	{
		if (user.uin == uin)
		{
			user.date = QDate::currentDate();
			return;
		}
	}

	UnknownUser newUser;
	newUser.uin = uin;
	newUser.date = QDate::currentDate();

	if (agentWidget != NULL)
	{
		SearchRecord searchUser;
		searchUser.clearData();

		QString uin_str = QString::number(newUser.uin);
		QString date_str = newUser.date.toString(Qt::ISODate);

		searchUser.reqUin(uin_str);
		gadu->searchNextInPubdir(searchUser);
		newUser.seq = searchUser.Seq;

		QStringList strings;
		strings << QString::null << date_str << QString::null << uin_str << QString::null << QString::null << QString::null << QString::null;
		agentWidget->Results->addTopLevelItem(new QTreeWidgetItem(agentWidget->Results, strings));
	}
	UnknownsList.append(newUser);

	UserListElement user;
	QString uin_str = QString::number(uin);
	user.addProtocol("Gadu", uin_str);

	UserListElements users(user);

	NewUserFoundNotification *notification = new NewUserFoundNotification(users);
	notification_manager->notify(notification);

	kdebugf2();
}

void Agent::lockupUser(UserListElements users)
{
	kdebugf();

	UserListElement user = *(users.constBegin());
	if(user.usesProtocol("Gadu"))
	{
		SearchDialog *sd;
		sd = new SearchDialog(kadu, user.ID("Gadu").toUInt());
		sd->show();
		sd->firstSearch();
	}

	kdebugf2();
}

void Agent::resultsRequest()
{
	kdebugf();

	if(agentWidget == NULL)
		agentWidget = new AgentWdg;

	agentWidget->show();

	kdebugf2();
}

void Agent::saveListToFile()
{
	kdebugf();

	QFile listFile;
	listFile.setName(QString(ggPath("agent-unknownslist").ascii()));

	listFile.remove();
	if(listFile.open(IO_WriteOnly))
	{
		QTextStream stream(&listFile);
		foreach(const UnknownUser &user, UnknownsList)
		{
			stream << user.uin << "," << user.date.toString(Qt::ISODate) << endl;
		}
		listFile.close();
	}

	kdebugf2();
}

void Agent::loadListFromFile()
{
	kdebugf();

	QFile listFile;
	listFile.setName(QString(ggPath("agent-unknownslist").ascii()));
	if (listFile.open(IO_ReadOnly))
	{
		QTextStream stream(&listFile);

		QString uin_str, date_str, line;
		bool ok;
		while (!stream.atEnd())
		{
			UnknownUser user;
			
			line = stream.readLine();
			uin_str = line.section(',', 0, 0);
			date_str = line.section(',', 1, 1);

			int uin_int = uin_str.toInt(&ok, 10);
			if(!ok)
				kdebugm(KDEBUG_PANIC, "Couldn't cast QString to int");

			user.uin = uin_int;
			user.date = QDate::fromString(date_str, Qt::ISODate);
			user.seq = 0;

			UnknownsList.append(user);
		}
		listFile.close();
	}

	kdebugf2();
}

void Agent::removeUserFromList(unsigned int uin_int)
{
	int index = 0;
	for (index = 0; index < UnknownsList.count(); ++index)
	{
		if (UnknownsList[index].uin == uin_int)
		{
			UnknownsList.removeAt(index);
			break;
		}
	}
}

AgentWdg::AgentWdg(QWidget * parent, const char * name, Qt::WindowFlags f) : QWidget(NULL, "agentwdg", Qt::WDestructiveClose)
{
	kdebugf();

	vBoxLayout1 = new QVBoxLayout(this, 4, 4, "vblayout1");
	infoLabel = new QLabel(this);
		infoLabel->setFrameStyle(QFrame::StyledPanel | QFrame::Raised);
		infoLabel->setText(tr("Warning:\nDetected are users, who has chosen option 'Private status'\nBelow is the list of users, who has you on their lists, but you don't have them on your list"));
		infoLabel->setAlignment(Qt::AlignAuto | Qt::AlignVCenter | Qt::WordBreak);
	vBoxLayout1->addWidget(infoLabel);

	Results = new QTreeWidget(this);
	Results->setAllColumnsShowFocus(true);

	QStringList labels;
	labels << tr("Status") << tr("Last seen") << tr("Description") << tr("Uin") << tr("Name") << tr("City") << tr("Nickname") << tr("Birth year");
	Results->setHeaderLabels(labels);
	Results->setColumnWidth(0, 60);
	Results->resize(800, this->height());

	vBoxLayout1->addWidget(Results);

	hBoxLayout1 = new QHBoxLayout(vBoxLayout1, 4, "hblayout1");
	openChat = new QPushButton(icons_manager->loadIcon("OpenChat"), tr("Open chat"), this);
	addContact = new QPushButton(icons_manager->loadIcon("AddUser"), tr("Add contact"), this);
	removeFromList = new QPushButton(icons_manager->loadIcon("DeleteUserlist"), tr("Remove from list"), this);

	hBoxLayout1->addWidget(removeFromList);
	hBoxLayout1->addWidget(addContact);
	hBoxLayout1->addWidget(openChat);

	connect(gadu, SIGNAL(newSearchResults(SearchResults &, int, int)), this, SLOT(pubdirResults(SearchResults &, int, int)));
	connect(removeFromList, SIGNAL(clicked()), this, SLOT(removeUser()));
	connect(openChat, SIGNAL(clicked()), this, SLOT(startChat()));
	connect(addContact, SIGNAL(clicked()), this, SLOT(addUser()));
	connect(Results, SIGNAL(itemSelectionChanged()), this, SLOT(userlistSelectionChanged()));

	removeFromList->setEnabled(false);
	openChat->setEnabled(false);
	addContact->setEnabled(false);

	foreach(UnknownUser user, agent->UnknownsList)
	{
		SearchRecord searchUser;
		searchUser.clearData();

		QString uin_str = QString::number(user.uin);
		QString date_str = user.date.toString(Qt::ISODate);

		searchUser.reqUin(uin_str);
		gadu->searchNextInPubdir(searchUser);
		user.seq = searchUser.Seq;

		QStringList strings;
		strings << QString::null << date_str << QString::null << uin_str << QString::null << QString::null << QString::null << QString::null;
		Results->addTopLevelItem(new QTreeWidgetItem(Results, strings));
	}

	this->adjustSize();
	this->setCaption(tr("Who has me on list?"));

	loadWindowGeometry(this, "Agent", "UsersFoundWidgetGeometry", 0, 50, 800, 150);

	kdebugf2();
};

AgentWdg::~AgentWdg()
{
	kdebugf();

	saveWindowGeometry(this, "Agent", "UsersFoundWidgetGeometry");

	disconnect(gadu, SIGNAL(newSearchResults(SearchResults &, int, int)), this, SLOT(pubdirResults(SearchResults &, int, int)));
	disconnect(removeFromList, SIGNAL(clicked()), this, SLOT(removeUser()));
	disconnect(openChat, SIGNAL(clicked()), this, SLOT(startChat()));
	disconnect(addContact, SIGNAL(clicked()), this, SLOT(addUser()));
	disconnect(Results, SIGNAL(itemSelectionChanged()), this, SLOT(userlistSelectionChanged()));

	delete Results;
	delete openChat;
	delete addContact;
	delete removeFromList;
	delete infoLabel;
	delete hBoxLayout1;
	delete vBoxLayout1;

	agentWidget = NULL;

	kdebugf2();
}

void AgentWdg::userlistSelectionChanged()
{
	if (Results->currentItem() == NULL)
	{
		removeFromList->setEnabled(false);
		openChat->setEnabled(false);
		addContact->setEnabled(false);
	}
	else
	{
		removeFromList->setEnabled(true);
		openChat->setEnabled(true);
		addContact->setEnabled(true);
	}
}

void AgentWdg::pubdirResults(SearchResults& searchResults, int seq, int fromUin)
{
	kdebugf();

	QTreeWidgetItem *qlv = 0;
	QPixmap pix;

	foreach(const SearchResult &searchResult, searchResults)
	{
		QList <QTreeWidgetItem *> items = Results->findItems(searchResult.Uin, Qt::MatchExactly, 3);
		if (items.count())
			qlv = items[0];

		pix = searchResult.Stat.pixmap(searchResult.Stat.status(), false, false);

		if (qlv)
		{
			qlv->setIcon(0, QIcon(pix));
			qlv->setText(2, searchResult.Stat.description());
			qlv->setText(3, searchResult.Uin);
			qlv->setText(4, searchResult.First);
			qlv->setText(5, searchResult.City);
			qlv->setText(6, searchResult.Nick);
			qlv->setText(7, searchResult.Born);
		}
		else
		{
			QStringList strings;
			strings << QString::null << searchResult.Stat.description() << searchResult.Uin << searchResult.First << searchResult.City << searchResult.Nick << searchResult.Born;
			qlv = new QTreeWidgetItem(Results, strings);
			qlv->setIcon(0, QIcon(pix));
//			qlv = 0;
		}
	}

	kdebugf2();
}


void AgentWdg::removeUser()
{
	kdebugf();

	QList<QTreeWidgetItem *> items = Results->selectedItems();
	if (items.count() == 0)
		return;

	QTreeWidgetItem *selectedEntry = items.first();

	bool ok;
	unsigned int uin_int = selectedEntry->text(2).toInt(&ok, 10);
	if (ok)
	{
		agent->removeUserFromList(uin_int);
	}

	delete selectedEntry;

	kdebugf2();
}

void AgentWdg::addUser()
{
	kdebugf();

	QList<QTreeWidgetItem *> items = Results->selectedItems();
	if (items.count() == 0)
		return;

	QTreeWidgetItem *selectedEntry = items.first();

	QString uin_str = selectedEntry->text(2);
	QString name = selectedEntry->text(3);
	QString nickname = selectedEntry->text(5);

	QString altnick = nickname;

	if (!altnick.length())
	{
		altnick = name;
	}

	if (!altnick.length())
		altnick = uin_str;

	UserListElement e;
	bool ok;
	e.setFirstName(name);
	e.setLastName("");
	e.setNickName(nickname);
	e.setAltNick(altnick);

	UinType uin = uin_str.toUInt(&ok);
	if(!ok)
		uin = 0;

	if(uin)
		e.addProtocol("Gadu", QString::number(uin));

	e.setEmail("");
	
	UserInfo *userinfo = new UserInfo(e, this);
	connect(userinfo, SIGNAL(updateClicked(UserInfo *)), this, SLOT(userInfoAccepted(UserInfo *)));
	userinfo->show();

	kdebugf2();
}

void AgentWdg::userInfoAccepted(UserInfo *info)
{
	UserListElement new_user = info->user();
	QString uin_str = new_user.ID("Gadu");

	bool ok;
	int uin_int = uin_str.toUInt(&ok);
	if(!ok)
		return;

	agent->removeUserFromList(uin_int);

	QList<QTreeWidgetItem *> items = Results->findItems(uin_str, Qt::MatchExactly, 2);
	QTreeWidgetItem *item = items.first();
	if (item != NULL)
	{
//		Results->takeItem(Results->takeTopLevelItem(currentIndex().row()));
		delete item;
	}

	disconnect(info, SIGNAL(updateClicked(UserInfo *)), this, SLOT(userInfoAccepted(UserInfo *)));
}

void AgentWdg::startChat()
{
	kdebugf();

	QList<QTreeWidgetItem *> items = Results->selectedItems();
	QTreeWidgetItem *selectedEntry = items.first();
	if (selectedEntry != NULL)
	{
		UinType uin = selectedEntry->text(2).toUInt();
		UserListElements users(userlist->byID("Gadu", QString::number(uin)));
	
		chat_manager->openPendingMsgs(users);
	}
	kdebugf2();
}

AgentWdg* agentWidget;
Agent* agent;
