#ifndef KADU_USERLIST_H
#define KADU_USERLIST_H

#include <qdns.h>
#include <qhostaddress.h>
#include <qobject.h>

#include "gadu.h"

class UserList;
class UserListElement;

class DnsHandler : public QObject
{
	Q_OBJECT

	public:
		DnsHandler(const QHostAddress &addr);
		~DnsHandler();

		static int counter;

	private:
		QDns DnsResolver;

	private slots:
		void resultsReady();

	signals:
		void result(const QString &hostname);
};

class UserListElement : public QObject
{
	Q_OBJECT
	private:
		QString Group;
		QString FirstName;
		QString LastName;
		QString NickName;
		QString AltNick;
		QString Mobile;
		QString Email;
		UinType Uin;
		UserStatus *Stat;
		int MaxImageSize;
		bool Anonymous;
		QHostAddress Ip;
		QString DnsName;
		short Port;
		int Version;
		bool Blocking;
		bool OfflineTo;
		bool Notify;

		UserList *Parent;
		friend class UserList;
	public slots:
		void setGroup(const QString& group);
		void setFirstName(const QString &firstName);
		void setLastName(const QString &lastName);
		void setNickName(const QString &nickName);
		void setAltNick(const QString &altNick);
		void setMobile(const QString &mobile);
		void setEmail(const QString &email);
		void setUin(const UinType &uin);
		void setMaxImageSize(const int maxImageSize);
		void setAnonymous(const bool anonymous);
		void setDnsName(const QString &dnsName);
		void setPort(short port);
		void setVersion(const int version);
		void setBlocking(const bool blocking);
		void setOfflineTo(const bool offlineTo);
		void setNotify(const bool notify);

	public:
		UserListElement(UserList* parent);
		UserListElement(const UserListElement &copyMe);
		UserListElement();
		virtual ~UserListElement();

		void refreshDnsName();

		void operator = (const UserListElement &copyMe);

		QString group() const;
		QString firstName() const;
		QString lastName() const;
		QString nickName() const;
		QString altNick() const;
		QString mobile() const;
		QString email() const;
		UinType uin() const;

		const UserStatus & status() const;
		UserStatus & status();

		int maxImageSize() const;
		bool isAnonymous() const;

		const QHostAddress & ip() const;
		QHostAddress & ip();

		QString dnsName() const;
		short port() const;
		int version() const;
		bool blocking() const;
		bool offlineTo() const;
		bool notify() const;
};

/**
	Klasa reprezentuj±ca listê kontaktów.
	indexami s± altnicki konwertowane tak,
	aby wszystkie litery by³y ma³e.
	Dziêki temu nie mog± istnieæ dwa kontakty
	o tych samych altnickach nawet, je¶li
	ró¿ni± siê wielko¶ciami liter.
	Pole Altnick w UserListElement zawiera natomiast
	oryginalny altnick.
	
	UWAGA! U¿ywanie metod klasy QMap nie jest zalecane,
	trzeba wtedy pamiêtaæ, ¿e jako klucza u¿ywamy altnicków
	odpowiednio skonwertowanych, ¿e zmiany powinny generowaæ
	odpowiednie sygna³y, itd. Du¿o lepiej u¿ywaæ specjalnie
	przygotowanych metod klasy UserList.
	
	TODO: Zgodnie z tym co powy¿ej z QMap powinni¶my dziedziczyæ
	prywatnie albo niech to bêdzie pole wewn±trz klasy. W tej
	chwili nie chcê ³amaæ api freeze w 0.4.0.
**/
class UserList : public QObject, public QMap<QString,UserListElement>
{
	Q_OBJECT

	protected:
		friend class UserListElement;

	public:
		UserList();
		UserList(const UserList& source);
		~UserList();
		UserList& operator=(const UserList& userlist);

		UserListElement& byUin(UinType uin);
		UserListElement& byNick(const QString& nickname);
		/**
			Wyszukuje kontakt po altnicku. Ma³e i
			du¿e litery nie maj± znaczenia.
		**/
		UserListElement& byAltNick(const QString& altnick);
		/**
			Wyszukuje kontakt po numerze uin.
			Jezeli nie znajdzie, zwracany jest
			UserListElement tylko z uin i altnick == uin
		**/
		UserListElement byUinValue(UinType uin);
		/**
			Sprawdza czy istnieje kontakt o danym numerze uin.
		**/
		bool containsUin(UinType uin) const;
		/**
			Sprawdza czy istnieje kontakt o danym altnicku.
			Ma³e i du¿e litery nie maj± znaczenia.
		**/
		bool containsAltNick(const QString& altnick) const;

	public slots:
		/**
			Dodaje do listy kontakt o podanych danych.
			Generuje nastêpnie sygna³y userDataChanged()
			i modified().
		**/
		void addUser(const UserListElement& ule);
		/**
			Dodaje do listy "anonimowy" kontakt o podanym
			numerze uin. Numer ten wykorzystywany jest jako
			altnick. Wywo³uje metodê addUser(), wiêc
			generowane s± sygna³y userDataChanged() i modified().
		**/
		void addAnonymous(UinType uin);
		/**
			Usuwa kontakt o podanym altnicku. Ma³e i
			du¿e litery nie maj± znaczenia.
			Tu¿ przed usuniêciem kontaktu z listy generuje
			sygna³ userDataChanged(). Po usuniêciu generowany
			jest sygna³ modified().
		**/
		void removeUser(const QString& altnick);
		/**
			Zmienia dane kontaktu o podanym altnicku. Ma³e i
			du¿e litery w zmiennej old_altnick nie maj± znaczenia.
			Po wykonaniu zmian wymusza ich uwzglêdnienie przez
			wszystkie obiekty klasy UserBox wywo³uj±c metodê
			UserBox::all_renameUser() i UserBox::all_refresh().
			Na koniec generuje sygna³y userDataChanged() i modified().
		**/		
		void changeUserInfo(const QString& old_altnick, const UserListElement& new_data);
		/**
			Zapisuje listê kontaktów do pliku o podanej nazwie
			(wraz ze ¶cie¿k±). Domy¶lnie jest to plik "userlist"
			w katalogu z ustawieniami.
			Utrzymywana jest kompatybilno¶æ z programem EKG,
			z tej przyczyny dodatkowe informacje jak to czy
			blokujemy u¿ytkownika, czy chcemy byæ powiadamiani
			o zmianach jego statusu itp. s± zapisywane do
			oddzielnego pliku "userattribs" w katalogu z
			ustawieniami.
			Kontakty "anonimowe" s± pomijane.
			
			TODO: To gdzie zapisujemy dodatkowe atrybutami te¿
			powinno byæ chyba podawane jako argument? Inaczej
			je¶li chcemy zapisaæ listê gdzie¶ indziej to nadpisz±
			nam siê atrybuty :/
		**/
		bool writeToFile(QString filename = "");
		/**
			Wczytuje listê kontaktów z plików "userlist"
			i "userattribs" z katalogu z ustawieniami.
			Metoda ta jest przeznaczona do przywracania
			danych zapisanych za pomoc± writeToFile().
		**/		
		bool readFromFile();
		void merge(const UserList &userlist);

	signals:
		/**
			Sygna³ generowany po zakoñczeniu zmian w li¶cie
			kontaktów.
		**/
		void modified();
		void dnsNameReady(UinType);
		/**
			Sygna³ jest generowany je¶li cokolwiek zmieni³o
			siê w danych którego¶ z kontaktów na li¶cie
			(wtedy przekazywane s± argumenty oldData i newData).
			Zmiana mo¿e nast±piæ przez bezpo¶rednie u¿ycie której¶
			z metod klasy UserListElement.
			Sygna³ przekazuje równie¿ informacjê o tym, ¿e nowy
			kontakt pojawi³ siê na li¶cie (wtedy oldData == NULL)
			lub ¿e zosta³ z listy usuniêty (wtedy newData = NULL).
		**/
		void userDataChanged(const UserListElement* const oldData, const UserListElement* const newData);
};

extern UserList userlist;

#endif

