#ifndef PENDING_MSGS_H
#define PENDING_MSGS_H

#include <qvaluelist.h>
#include "gadu.h"
#include "misc.h"

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
			UinsList uins; /*!< Lista numerów UIN, które nades³a³y tê wiadomo¶æ. */
			QString msg; /*!< Tre¶æ wiadomo¶ci. */
			int msgclass; /*!< Klasa wiadomo¶ci. */
			time_t time; /*!< Czas, w którym nadano wiadomo¶æ. */
		};

	private:
		typedef QValueList<Element> PendingMsgsList;
		PendingMsgsList msgs;
		
	public:
		/**
			\fn PendingMsgs(QObject *parent=0, const char *name=0)
			Standardowy konstruktor.
			\param parent rodzic kontrolki. Domy¶lnie 0.
			\param name nazwa kontrolki. Domy¶lnie 0.
		**/
		PendingMsgs(QObject *parent=0, const char *name=0);

		/**
			\fn bool pendingMsgs(UinType uin) const
			Sprawdza, czy w kolejce s± jakie¶ oczekuj±ce wiadomo¶ci dla podanego numeru UIN.
			\param uin numer UIN dla którego nale¿y sprawdziæ kolejkê.
			\return warto¶æ logiczn± informuj±c± czy w kolejce jest jaka¶ wiadomo¶æ czy nie.
		**/
		bool pendingMsgs(UinType uin) const;

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
			void addMsg(UinsList uins, QString msg, int msgclass, time_t time)
			Dodaje now± wiadomo¶æ do kolejki.
			\param uins lista numerów UIN, które przys³a³y wiadomo¶æ.
			\param msg tre¶æ wiadomo¶ci.
			\param msgclass klasa wiadomo¶ci.
			\param time czas nades³ania wiadomo¶ci.
		**/
		void addMsg(UinsList uins, QString msg, int msgclass, time_t time);

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
			\fn void messageAdded()
			Sygna³ ten emitowany jest, gdy nowa wiadomo¶æ zosta³a dodana do kolejki.
		**/
		void messageAdded();

		/**
			\fn void messageDeleted()
			Sygna³ ten emitowany jest, gdy dowolna wiadomo¶æ zosta³a usuniêta z kolejki.
		**/
		void messageDeleted();
};

extern PendingMsgs pending;

#endif
