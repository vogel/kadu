#ifndef PENDING_MSGS_H
#define PENDING_MSGS_H

#define QT3_SUPPORT
#include <qglobal.h>

#include <qstring.h>
#include <q3valuelist.h>
#include <time.h>
#include "usergroup.h"

/**
	Klasa ta reprezentuje kolejkê wiadomo¶ci oczekuj±cych na odczytanie.
	Wiadomo¶ci takie mog± byæ przechowywane w tej klasie nawet pomiêdzy kolejnymi
	uruchomieniami Kadu.
	\class PendingMsgs
	\brief Kolejka oczekuj±cych wiadomo¶ci.
**/
class PendingMsgs : public QObject
{
	Q_OBJECT

	public:
		/**
			Struktura reprezentuj±ca wiadomo¶æ oczekuj±c± w kolejce.
			\struct Element
			\brief Wiadomo¶æ oczekuj±c± w kolejce.
		**/
		struct Element
		{
			UserListElements users; /*!< Lista u¿ytkowników, którzy nades³ali tê wiadomo¶æ. */
			QString proto; /*!< Nazwa protoko³u. */
			QString msg; /*!< Tre¶æ wiadomo¶ci. */
			int msgclass; /*!< Klasa wiadomo¶ci. */
			time_t time; /*!< Czas, w którym nadano wiadomo¶æ. */
			Element();
		};

	private:
		typedef Q3ValueList<Element> PendingMsgsList;
		PendingMsgsList msgs;

	public:
		/**
			\fn PendingMsgs(QObject *parent=0, const char *name=0)
			Standardowy konstruktor.
			\param parent rodzic kontrolki. Domy¶lnie 0.
			\param name nazwa kontrolki. Domy¶lnie 0.
		**/
		PendingMsgs(QObject *parent = 0, const char *name = 0);

		/**
			\fn bool pendingMsgs(UserListElement user) const
			Sprawdza, czy w kolejce s± jakie¶ oczekuj±ce wiadomo¶ci dla podanego u¿ytkownika.
			\param user u¿ytkownik, dla którego nale¿y sprawdziæ kolejkê.
			\return warto¶æ logiczn± informuj±c± czy w kolejce jest jaka¶ wiadomo¶æ czy nie.
		**/
		bool pendingMsgs(UserListElement user) const;

		/**
			\fn bool pendingMsgs() const
			Sprawdza, czy w jakiejkolwiek kolejce s± oczekuj±ce wiadomo¶ci.
			\return warto¶æ logiczn± informuj±c± czy s± jaka¶ wiadomo¶ci czy nie.
		**/
		bool pendingMsgs() const;

		/**
			\fn int count() const
			Liczy wszystkie wiadomo¶ci oczekuj±ce w kolejkach.
			\return liczba oczekuj±cych wiadomo¶ci.
		**/
		int count() const;

		/**
			\fn Element &operator[](int index)
			\return wiadomo¶æ z kolejki, o podanym indeksie.
		**/
		Element &operator[](int index);

	public slots:
		/**
			\fn void openMessages()
			Wywo³uje metodê ChatManager::openPendingMsgs.
		**/
		void openMessages();

		/**
			\fn void deleteMsg(int index)
			Usuwa z kolejki wiadomo¶æ o danym indeksie.
			\param index indeks wiadomo¶ci do usuniêcia.
		**/
		void deleteMsg(int index);

		/**
			\fn void addMsg(QString protocolName, UserListElements users, QString msg, int msgclass, time_t time)
			Dodaje now± wiadomo¶æ do kolejki.
			\param protocolName nazwa protoko³u
			\param users lista u¿ytkowników, które przys³a³y wiadomo¶æ.
			\param msg tre¶æ wiadomo¶ci.
			\param msgclass klasa wiadomo¶ci.
			\param time czas nades³ania wiadomo¶ci.
		**/
		void addMsg(QString protocolName, UserListElements users, QString msg, int msgclass, time_t time);

		/**
			\fn bool loadFromFile()
			Wczytuje kolejkê wiadomo¶ci z pliku. Metoda ta wywo³ywana jest przy starcie
			Kadu.
		**/
		bool loadFromFile();

		/**
			\fn bool loadFromFile()
			Zapisuje kolejkê wiadomo¶ci do pliku. Metoda ta wywo³ywana jest przy zamykaniu
			Kadu.
		**/
		void writeToFile();

	signals:
		/**
			\fn void messageFromUserAdded(UserListElement user)
			Sygna³ ten emitowany jest, gdy nowa wiadomo¶æ zosta³a dodana do kolejki.
		**/
		void messageFromUserAdded(UserListElement user);

		/**
			\fn void messageFromUserDeleted(UserListElement user)
			Sygna³ ten emitowany jest, gdy wiadomo¶æ od u¿ytkownika user zosta³a usuniêta z kolejki.
		**/
		void messageFromUserDeleted(UserListElement user);
};

extern PendingMsgs pending;

#endif
