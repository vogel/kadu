
#ifndef PENDING_EVENTS_H
#define PENDING_EVENTS_H

#include <qptrlist.h>
#include "gadu.h"

/**
	Klasa bazowa dla wszystkich kolejkowanych zdarzeñ.
	\class PendingEvent
	\brief Zdarzenia kolejkowane.
**/
class PendingEvent
{
	public:
		/**
			\fn virtual bool sendBy(UinType uin)
			Sprawdza czy zdarzenie dotyczy podanego numeru UIN.
			\param uin numer UIN o który pytamy.
			\return warto¶æ logiczna informuj±ca czy zdarzenie dotyczy tego numeru UIN czy nie.
		**/
		virtual bool sendBy(UinType uin)=0;

		/**
			\fn virtual void activate()
			Czyni zdarzenie aktywnym.
		**/
		virtual void activate()=0;
};

/**
	\class PendingMessage
	\brief Wiadomo¶æ kolejkowana.
**/
class PendingMessage : public PendingEvent
{
	private:
		UinsList Uins;
		QString Msg;
		int MsgClass;
		time_t Time;
		
	public:
		/**
			\fn PendingMessage(const UinsList uins,const QString& msg, const int msg_class,const time_t time)
			Konstruktor tworz±cy wiadomo¶æ kolejkowan± wg. podanych danych.
			\param uins lista numerów UIN od których otrzymano t± wiadomo¶æ.
			\param msg tre¶æ wiadomo¶ci.
			\param msg_class klasa wiadomo¶ci.
			\param time czas wys³ania wiadomo¶ci.
		**/
		PendingMessage(const UinsList uins,const QString& msg,
			const int msg_class,const time_t time);
		virtual ~PendingMessage();

		/**
			\fn virtual bool sendBy(UinType uin)
			Sprawdza czy zdarzenie dotyczy podanego numeru uin.
			Jest to prawda jesli podany uin jest jednym z
			nadawcow wiadomosci.
			\param uin numer UIN o który pytamy.
			\return warto¶æ logiczna informuj±ca czy zdarzenie dotyczy tego numeru UIN czy nie.
		**/
		virtual bool sendBy(UinType uin);

		/**
			\fn virtual void activate()
			Aktywuje zdarzenie. Wy¶wietla wiadomo¶æ w oknie Chat.
		**/
		virtual void activate();
};

/**
	Klasa bazowa dla wszystkich kolejek zdarzeñ.
	\class PendingEvents
	\brief Kolejki zdarzeñ.
**/
class PendingEvents : public QObject
{
	Q_OBJECT

	private:
		typedef QPtrList<PendingEvent> PendingEventsList;
		PendingEventsList Events;
		
	public:
		/**
			\fn PendingEvents()
			Standardowy konstruktor.
		**/
		PendingEvents();

		/**
			\fn void queueEvent(PendingEvent* event)
			Kolejkuje zdarzenie.
			\param event wska¼nik do obiektu,
				który jest usuwany przez PendingEvents
				podczas aktywacji zdarzenia.
		**/
		void queueEvent(PendingEvent* event);

		/**
			\fn void activateEvent(int index)
			Aktywuje zakolejkowane zdarzenie o podanym indeksie
			i usuwa je z kolejki.
			\param index indeks zdarzenia, które ma byæ aktywowane i usuniête.
		**/
		void activateEvent(int index);

		/**
			\fn void activateEvent()
			Aktywuje i usuwa z kolejki pierwsze zakolejkowane
			zdarzenie.
		**/
		void activateEvent();

		/**
			\fn PendingEvent* operator[](int index)
			\param index index zakolejkowanego zdarzenia.
			\return zakolejkowane zdarzenie o podanym indeksie.
		**/
		PendingEvent* operator[](int index);

		/**
			\fn bool pendingEvents(UinType uin)
			Sprawdza czy istniej± zakolejkowane zdarzenia
			dotycz±ce podanego numeru uin.
			\param uin numer UIN o który pytamy.
			\return warto¶æ logiczna informuj±ca czy istniej± zakolejkowane zdarzenia.
		**/
		bool pendingEvents(UinType uin);

		/**
			\fn bool pendingEvents()
			Sprawdza czy istnieja jakiekolwiek zakolejkowane zdarzenia.
			\return warto¶æ logiczna informuj±ca czy istniej± zakolejkowane zdarzenia.
		**/
		bool pendingEvents();
//		bool loadFromFile();
//		void writeToFile();

	signals:
		/**
			\fn void eventQueued(PendingEvent* event)
			Sygnal emitowany po zakolejkowaniu zdarzenia.
			\param event wska¼nik do obiektu opisuj±cego zdarzenie.
		**/
		void eventQueued(PendingEvent* event);

		/**
			\fn void eventActivated(PendingEvent* event)
			Sygnal emitowany po aktywacji zdarzenia i usuniêciu
			go z kolejki. Po obs³u¿eniu sygna³u obiekt jest
			usuwany.
			\param event wska¼nik do obiektu opisuj±cego zdarzenie.
		**/
		void eventActivated(PendingEvent* event);
};

extern PendingEvents pending_events;

#endif
