
#include <qfile.h>
#include <qtextcodec.h>

#include "pending_events.h"
#include "debug.h"
#include "kadu.h"
//#include "config_file.h"
#include "chat.h"


PendingMessage::PendingMessage(const UinsList uins,const QString& msg,
			const int msg_class,const time_t time)
{
	Uins=uins;
	Msg=msg;
	MsgClass=msg_class;
	Time=time;
}

PendingMessage::~PendingMessage()
{
}

bool PendingMessage::sendBy(UinType uin)
{
	for(UinsList::iterator i=Uins.begin(); i!=Uins.end(); ++i)
		if((*i)==uin)
			return true;
	return false;
}

void PendingMessage::activate()
{
	bool msgsFromHist = false;
	QString toadd;
	
	if ((MsgClass & GG_CLASS_CHAT) == GG_CLASS_CHAT || (MsgClass & GG_CLASS_MSG) == GG_CLASS_MSG
		|| (!MsgClass))
	{
		for (int i = 0; i < Uins.count(); ++i)
			if (!userlist.containsUin(Uins[i]))
			{
				QString tmp = QString::number(Uins[i]);
				UserListElement e;
				e.first_name = "";
				e.last_name = "";
				e.nickname = tmp;
				e.altnick = tmp;
				e.mobile = "";
				e.uin = Uins[i];
				e.setGroup("");
				e.description = "";
				e.email = "";
				e.anonymous = true;
				if (useDocking)
					userlist.addUser(e);
				else
					kadu->addUser(e);
			}
		int k = kadu->openChat(Uins);
		if (!msgsFromHist)
		{
			msgsFromHist = true;
			chats[k].ptr->writeMessagesFromHistory(Uins, Time);
		}
		chats[k].ptr->formatMessage(false, userlist.byUin(Uins[0]).altnick, Msg, timestamp(Time), toadd);
	}
}

PendingEvents::PendingEvents() : QObject()
{
}

void PendingEvents::queueEvent(PendingEvent* event)
{
	Events.append(event);
//	writeToFile();
	emit eventQueued(event);
}

void PendingEvents::activateEvent(int index)
{
	kassert(index>=0);
	kassert(index<Events.count());
	kdebugm(KDEBUG_INFO, "PendingEvents::(pre)activateEvent(%d), count=%d\n", index, Events.count());
	PendingEvent* event=Events.at(index);
	event->activate();
	Events.remove(index);
//	writeToFile();
	kdebugm(KDEBUG_INFO, "PendingEvents::activateEvent(%d), count=%d\n", index, Events.count());
	emit eventActivated(event);
	delete event;
}

void activateEvent()
{
	activateEvent(0);
}

PendingEvent* PendingEvents::operator[](int index)
{
	return Events.at(index);
}

bool PendingEvents::pendingEvents(UinType uin)
{
	for(PendingEventsList::iterator i=Events.begin(); i!=Events.end(); ++i)
		if((*i)->sendBy(uin))
			return true;
	return false;
}

bool PendingEvents::pendingEvents()
{
	return (Events.count()>0);
}

extern PendingEvents pending_events;
