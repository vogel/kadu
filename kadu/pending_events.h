
#ifndef PENDING_EVENTS_H
#define PENDING_EVENTS_H

#include <qptrlist.h>
#include "misc.h"

class PendingEvent
{
	public:
		virtual bool sendBy(uin_t uin)=0;
		virtual void activate()=0;
};

class PendingMessage : public PendingEvent
{
	private:
		UinsList Uins;
		QString Msg;
		int MsgClass;
		time_t Time;
		
	public:
		PendingMessage(const UinsList uins,const QString& msg,
			const int msg_class,const time_t time);
		virtual bool sendBy(uin_t uin);
		virtual void activate();
};

class PendingEvents : public QObject
{
	Q_OBJECT

	private:
		typedef QPtrList<PendingEvent> PendingEventsList;
		PendingEventsList Events;
		
	public:
		PendingEvents();
		void queueEvent(PendingEvent* event);
		void activateEvent(int index);
		PendingEvent* operator[](int index);
		bool pendingEvents(uin_t uin);
		bool pendingEvents();
//		bool loadFromFile();
//		void writeToFile();

	signals:
		void eventQueued(PendingEvent* event);
		void eventActivated(PendingEvent* event);
};

extern PendingEvents pending_events;

#endif
