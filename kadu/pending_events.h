
#ifndef PENDING_EVENTS_H
#define PENDING_EVENTS_H

#include <qptrlist.h>
#include "gadu.h"
#include "misc.h"

/**
	Klasa bazowa dla wszystkich kolejkowanych zdarzen.
**/
class PendingEvent
{
	public:
		/**
			Sprawdza czy zdarzenie dotyczy podanego numeru uin.
		**/
		virtual bool sendBy(UinType uin)=0;
		/**
			Aktywuje zdarzenie.
		**/
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
		virtual ~PendingMessage();
		/**
			Sprawdza czy zdarzenie dotyczy podanego numeru uin.
			Jest to prawda jesli podany uin jest jednym z
			nadawcow wiadomosci.
		**/
		virtual bool sendBy(UinType uin);
		/**
			Aktywuje zdarzenie. Wyswietla wiadomosc w oknie Chat.
		**/		
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
		/**
			Kolejkuje zdarzenie. Obiekt do ktorego wskaznik
			przekazujemy jest usuwany przez PendingEvents
			podczas aktywacji zdarzenia.
		**/
		void queueEvent(PendingEvent* event);
		/**
			Aktywuje zakolejkowane zdarzenie o podanym indeksie
			i usuwa je z kolejki.
		**/				
		void activateEvent(int index);
		/**
			Aktywuje i usuwa z kolejki pierwsze zakolejkowane
			zdarzenie.
		**/				
		void activateEvent();
		/**
			Zwraca zakolejkowane zdarzenie o podanym indeksie.
		**/						
		PendingEvent* operator[](int index);
		/**
			Sprawdza czy istnieja zakolejkowane zdarzenia
			dotyczace podanego numeru uin.
		**/		
		bool pendingEvents(UinType uin);
		/**
			Sprawdza czy istnieja zakolejkowane zdarzenia.
		**/				
		bool pendingEvents();
//		bool loadFromFile();
//		void writeToFile();

	signals:
		/**
			Sygnal emitowany po zakolejkowaniu zdarzenia.
		**/					
		void eventQueued(PendingEvent* event);
		/**
			Sygnal emitowany po aktywacji zdarzenia i usunieciu
			go z kolejki. Po obsluzeniu sygnalu obiekt jest
			usuwany.
		**/							
		void eventActivated(PendingEvent* event);
};

extern PendingEvents pending_events;

#endif
