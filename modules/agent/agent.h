/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef AGENT_H
#define AGENT_H

#include <QtCore/QObject>
#include <QtGui/QLayout>
#include <QtGui/QTreeWidget>
#include <QtGui/QTreeWidgetItem>
#include <QtGui/QPushButton>
#include <QtCore/QList>
#include <QtGui/QLabel>

#include "../gadu_protocol/gadu.h"
#include "userinfo.h"

struct UnknownUser
{
	UinType uin;
	QDate date;
	int seq;
};

class ActionDescription;

class AgentWdg : public QWidget
{
	Q_OBJECT

	friend class Agent;

	public:
		AgentWdg(QWidget * parent = 0, const char * name = 0, Qt::WindowFlags f = 0);
		~AgentWdg();

	private:
		QTreeWidget* Results;
		QPushButton* addContact;
		QPushButton* removeFromList;
		QPushButton* openChat;
		QLabel* infoLabel;

		QVBoxLayout* vBoxLayout1;
		QHBoxLayout* hBoxLayout1;
	private slots:
		void pubdirResults(SearchResults& searchResults, int seq, int fromUin);
		void removeUser();
		void addUser();
		void startChat();
		void userlistSelectionChanged();
		void userInfoAccepted(UserInfo *info);
};

class Agent : public QObject
{
	Q_OBJECT

	friend class AgentWdg;
	friend class NewUserFoundNotification;

	public:
		Agent();
		~Agent();

		void removeUserFromList(unsigned int uin_int);

	private:
		QList<UnknownUser> UnknownsList;
		int menuItemId;
		ActionDescription *agentActionDescription;

		void saveListToFile();
		void loadListFromFile();
		void lockupUser(UserListElements users);

	private slots:
		void userFound(UinType uin);
		void resultsRequest();
};

extern Agent* agent;
extern AgentWdg* agentWidget;

#endif
