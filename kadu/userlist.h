#ifndef KADU_USERLIST_H
#define KADU_USERLIST_H

#include <qdns.h>
#include <qhostaddress.h>
#include <qobject.h>

#include "gadu.h"

class UserList;
class UserListElement;

/**
	T³umaczy adres IP na nazwê domeny (DNS). Kiedy proces zostanie zakoñczony,
	emituje odpowiedni sygna³ zawieraj±cy informacjê o nazwie domeny.
	\class DnsHandler
	\brief Klasa t³umacz±ca adres IP na nazwê domeny.
**/
class DnsHandler : public QObject
{
	Q_OBJECT

	public:
		/**
			\fn DnsHandler(const QHostAddress &addr);
			Konstruktor wywo³uj±cy zapytanie o domenê dla danego adresu IP.
			\param addr adres IP
		**/
		DnsHandler(const QHostAddress &addr);
		~DnsHandler();

		static int counter; /*!< licznik obiektów tej klasy */

	private:
		QDns DnsResolver; /*!< obiekt zajmuj±cy siê translacj± adresu na nazwê domeny */

	private slots:
		/**
			\fn void resultsReady()
			Funkcja wywo³ywana, gdy proces t³umaczenia zosta³ zakoñczony.
		**/
		void resultsReady();

	signals:
		/**
			\fn void result(const QString &hostname)
			Sygna³ emitowany, gdy proces t³umaczenia zosta³ zakoñczony.
			\param hostname nazwa domeny odpowiadaj±ca adresowi IP o który pytano
		**/
		void result(const QString &hostname);
};

/**
	Typ opisuj±cy stan powiadamiania.
	\enum NotifyType
	\brief Typ opisuj±cy stan powiadamiania.
**/
enum NotifyType {
	GLOBAL = 0, /*!< powiadomienia zale¿ne od globalnych ustawieñ */
	OFF = 1, /*!< powiadomienia wy³±czone */
	OWN = 2 /*!< indywidualne ustawienie powiadamiania */
};

/**
	Klasa reprezentuj±ca kontakt (u¿ytkownika na li¶cie kontaktów).
	\class UserListElement
	\brief Klasa reprezentuj±ca kontakt.
**/
class UserListElement : public QObject
{
	Q_OBJECT
	private:
		QString Group; /*!< Grupa do której przynale¿y kontakt
						(je¶li wiêcej ni¿ jedna, to oddzielone s± przecinkami). */
		QString FirstName; /*!< Imiê kontaktu. */
		QString LastName; /*!< Nazwisko kontaktu. */
		QString NickName; /*!< Pseudonim kontaktu. */
		QString AltNick; /*!< Pseudonim kontaktu, który jest wy¶wietlany na li¶cie. */
		QString Mobile; /*!< Numer telefonu kontaktu. */
		QString Email; /*!< E-Mail kontaktu. */
		UinType Uin; /*!< UIN kontaktu. */
		UserStatus *Stat; /*!< Aktualny status kontaktu. */
		int MaxImageSize; /*!< Maksymalny rozmiar obrazka, jak± mo¿e przyj±æ kontakt. */
		bool Anonymous; /*!< Informuje, czy kontakt jest anonimowy czy nie. */
		QHostAddress Ip; /*!< Adres IP kontaktu (je¶li wykryty). */
		QString DnsName; /*!< Nazwa domenu kontaktu (je¶li znaleziona). */
		short Port; /*!< Port kontaktu (je¶li wykryty). */
		int Version; /*!< Wersja protoko³u u¿ywanego przez kontakt. */
		bool Blocking; /*!< Informuje czy blokujemy kontakt, czy nie. */
		bool OfflineTo; /*!< Informuje czy mamy w³±czony tryb "niedostêpny dla kontaktu" dla tego kontaktu. */
		bool Notify; /*!< Informuje czy mamy w³±czone powiadamianie o kontakcie. */
		
		NotifyType AliveSound; /*!< Przechowuje informacjê o sposobie powiadamiania
								o zmianie statusu kontaku d¼wiêkiem. */
		QString OwnAliveSound; /*!< Je¶li sposób powiadamiania o zmianie statusu kontaktu ma warto¶æ OWN,
								to ta zmienna przechowuje nazwê pliku d¼wiêkowego do odtworzenia. */

		NotifyType MessageSound; /*!< Przechowuje informacjê o sposobie powiadamiania
								o nowej wiadomo¶ci od kontaktu d¼wiêkiem. */
		QString OwnMessageSound; /*!< Je¶li sposób powiadamiania o nowej wiadomo¶ci od kontaktu ma warto¶æ OWN,
								to ta zmienna przechowuje nazwê pliku d¼wiêkowego do odtworzenia. */
		
		QString HomePhone; /*!< Numer telefonu domowego kontaktu. */

		UserList *Parent; /*!< Wska¼nik do listy kontaktów, na której znajduje siê ten kontakt. */
		friend class UserList;
	public slots:
		/**
			\fn void setGroup(const QString& group)
			Ustawia grupê (lub grupy, oddzielone przecinkami) dla kontaktu.
			\param group grupa (grupy) do której zostanie przydzielony kontakt.
		**/
		void setGroup(const QString& group);
		
		/**
			\fn void setFirstName(const QString &firstName)
			Ustawia imiê dla kontaktu.
			\param firstName imiê, które zostanie przydzielone kontaktowi.
		**/
		void setFirstName(const QString &firstName);
		
		/**
			\fn void setLastName(const QString &lastName)
			Ustawia nazwisko dla kontaktu.
			\param lastName nazwisko, które zostanie przydzielone kontaktowi.
		**/
		void setLastName(const QString &lastName);
		
		/**
			\fn void setNickName(const QString &nickName)
			Ustawia pseudonim dla kontaktu.
			\param nickName pseudonim, który zostanie przydzielony kontaktowi.
		**/
		void setNickName(const QString &nickName);
		
		/**
			\fn void setAltNick(const QString &altNick)
			Ustawia wy¶wietlany pseudonim dla kontaktu.
			\param altNick wy¶wietlany pseudonim, który zostanie przydzielony kontaktowi.
		**/
		void setAltNick(const QString &altNick);
		
		/**
			\fn void setMobile(const QString &mobile)
			Ustawia numer telefonu dla kontaktu.
			\param mobile numer telefonu, który zostanie przydzielony kontaktowi.
		**/
		void setMobile(const QString &mobile);
		
		/**
			\fn void setEmail(const QString &email)
			Ustawia adres E-Mail dla kontaktu.
			\param email adres, który zostanie przydzielony kontaktowi.
		**/
		void setEmail(const QString &email);
		
		/**
			\fn void setUin(const UinType &uin)
			Ustawia UIN dla kontaktu.
			\param uin numer UIN, który zostanie przydzielony kontaktowi.
		**/
		void setUin(const UinType &uin);
		
		/**
			\fn void setMaxImageSize(const int maxImageSize)
			Ustawia maksymalny rozmiar obrazka dla kontaktu.
			\param maxImageSize rozmiar obrazka w bajtach, który zostanie przydzielony kontaktowi.
		**/
		void setMaxImageSize(const int maxImageSize);
		
		/**
			\fn void setAnonymous(const bool anonymous)
			Ustawia stan anonimowo¶ci kontaktu.
			\param anonymous warto¶æ logiczna informuj±ca, ¿e kontakt jest anonimowy, lub nie.
		**/
		void setAnonymous(const bool anonymous);
		
		/**
			\fn void setDnsName(const QString &dnsName)
			Ustawia nazwê domeny dla kontaktu.
			\param dnsName nazwa domenu, która zostanie przydzielona kontaktowi.
		**/
		void setDnsName(const QString &dnsName);
		
		/**
			\fn void setPort(short port)
			Ustawia numer portu dla kontaktu.
			\param port numer portu, który zostanie przydzielony kontaktowi.
		**/
		void setPort(short port);
		
		/**
			\fn void setVersion(const int version)
			Ustawia wersjê protoko³u dla kontaktu.
			\param version wersja protoko³u, która zostanie przydzielona kontaktowi.
		**/
		void setVersion(const int version);
		
		/**
			\fn void setBlocking(const bool blocking)
			Ustawia stan blokowania kontaktu.
			\param blocking warto¶æ logiczna informuj±ca, czy kontakt ma byæ blokowany.
		**/
		void setBlocking(const bool blocking);
		
		/**
			\fn void setOfflineTo(const bool offlineTo)
			Ustawia stan niedostêpno¶ci dla kontaktu.
			\param offlineTo warto¶æ logiczna informuj±ca, czy u¿ytkownik ma byæ "niedostêpny dla kontaktu".
		**/
		void setOfflineTo(const bool offlineTo);
		
		/**
			\fn void setNotify(const bool notify)
			Ustawia stan powiadamiania o kontakcie.
			\param notify warto¶æ logiczna informuj±ca, czy u¿ytkownik ma byæ powiadamiany o kontakcie.
		**/
		void setNotify(const bool notify);

		/**
			\fn void setHomePhone(const QString &phone)
			Ustawia numer telefonu domowego dla kontaktu.
			\param phone numer telefonu, który zostanie przydzielony kontaktowi.
		**/
		void setHomePhone(const QString &phone);
		
		/**
			\fn void setAliveSound(NotifyType type, const QString &file="")
			Ustawia sposób powiadamiania d¼wiêkiem o zmianie statusu przez kontakt.
			\param type sposób powiadamiania.
			\arg \c GLOBAL powiadomienia zale¿ne od globalnych ustawieñ.
			\arg \c OFF powiadomienia wy³±czone.
			\arg \c OWN indywidualne ustawienie powiadamiania/

			\param file plik d¼wiêkowy, wymagany dla indywidualnego ustawienia powiadomieñ.
		**/
		void setAliveSound(NotifyType type, const QString &file="");
		
		/**
			\fn void setMessageSound(NotifyType type, const QString &file="")
			Ustawia sposób powiadamiania d¼wiêkiem o nowej wiadomo¶ci od kontaktu.
			\param type sposób powiadamiania.
			\arg \c GLOBAL powiadomienia zale¿ne od globalnych ustawieñ.
			\arg \c OFF powiadomienia wy³±czone.
			\arg \c OWN indywidualne ustawienie powiadamiania/

			\param file plik d¼wiêkowy, wymagany dla indywidualnego ustawienia powiadomieñ.
		**/
		void setMessageSound(NotifyType type, const QString &file="");

	public:
		/**
			\fn UserListElement(UserList* parent)
			Konstruktor dodaj±cy kontakt do wskazanej listy kontaktów.
			\param parent wska¼nik listy kontaktów, do której zostanie dodany kontakt.
		**/
		UserListElement(UserList* parent);

		/**
			\fn UserListElement(const UserListElement &copyMe)
			Konstruktor kopiuj±cy.
		**/
		UserListElement(const UserListElement &copyMe);

		/**
			\fn UserListElement()
			Domy¶lny konstruktor tworz±cy kontakt o nie wype³nionych jeszcze danych.
		**/
		UserListElement();
		virtual ~UserListElement();

		/**
			\fn void refreshDnsName()
			Wywo³uje zapytanie o nazwê domeny dla kontaktu oraz wype³nia odpowiednie pole kontaktu,
			gdy domena zostanie odnaleziona.
		**/
		void refreshDnsName();

		/**
			\fn void operator = (const UserListElement &copyMe)
			Operator kopiuj±cy.
			\param copyMe obiekt klasy UserListElement który bêdzie skopiowany.
		**/
		void operator = (const UserListElement &copyMe);

		/**
			\fn const QString &group() const
			Zwraca grupê (lub wiele grup oddzielonych przecinkami) do której nale¿y kontakt.
		**/
		const QString &group() const;
		
		/**
			\fn const QString &firstName() const
			Zwraca imiê kontaktu.
		**/
		const QString &firstName() const;
		
		/**
			\fn const QString &lastName() const
			Zwraca nazwisko kontaktu.
		**/
		const QString &lastName() const;
		
		/**
			\fn const QString &nickName() const
			Zwraca pseudonim kontaktu.
		**/
		const QString &nickName() const;
		
		/**
			\fn const QString &altNick() const
			Zwraca wy¶wietlany pseudonim kontaktu.
		**/
		const QString &altNick() const;
		
		/**
			\fn const QString &mobile() const
			Zwraca numer telefonu kontaktu.
		**/
		const QString &mobile() const;
		
		/**
			\fn const QString &email() const
			Zwraca adres E-Mail kontaktu.
		**/
		const QString &email() const;
		
		/**
			\fn UinType uin() const
			Zwraca numer UIN kontaktu.
		**/
		UinType uin() const;
		
		/**
			\fn const QString &homePhone() const
			Zwraca numer telefonu domowego kontaktu.
		**/
		const QString &homePhone() const;
		
		/**
			\fn const QString &aliveSound(NotifyType &type) const
			Zwraca sposób powiadamiania d¼wiêkiem o zmianach statusu kontaktu.
		**/
		const QString &aliveSound(NotifyType &type) const;
		
		/**
			\fn const QString &messageSound(NotifyType &type) const
			Zwraca sposób powiadamiania d¼wiêkiem o wiadomo¶ciach od kontaktu.
		**/
		const QString &messageSound(NotifyType &type) const;

		/**
			\fn const UserStatus & status() const
			Zwraca aktualny status kontaktu.
		**/
		const UserStatus & status() const;
		
		/**
			\overload UserStatus & status()
		**/
		UserStatus & status();

		/**
			\fn int maxImageSize() const
			Zwraca maksymalny rozmiar obrazka dla kontaktu.
		**/
		int maxImageSize() const;
		
		/**
			\fn bool isAnonymous() const
			Zwraca informacjê, czy kontakt jest anonimowy czy nie.
		**/
		bool isAnonymous() const;

		/**
			\fn const QHostAddress & ip() const
			Zwraca adres IP kontaktu (je¶li wykryty).
		**/
		const QHostAddress & ip() const;
		
		/**
			\overload QHostAddress & ip()
		**/
		QHostAddress & ip();
		
		/**
			\fn const QString &dnsName() const
			Zwraca nazwê domeny kontaktu (je¶li znaleziona).
		**/
		const QString &dnsName() const;
		
		/**
			\fn short port() const
			Zwraca numer portu kontaktu (je¶li znaleziony).
		**/
		short port() const;
		
		/**
			\fn int version() const
			Zwraca wersjê protoko³u, jakiej u¿ywa kontakt.
		**/
		int version() const;
		
		/**
			\fn bool blocking() const
			Zwraca informacjê, czy kontakt jest blokowany czy nie.
		**/
		bool blocking() const;
		
		/**
			\fn bool offlineTo() const
			Zwraca informacjê, czy u¿ytkownik jest "niedostêpny dla kontaktu" czy nie.
		**/
		bool offlineTo() const;
		
		/**
			\fn bool notify() const
			Zwraca informacjê, czy u¿ytkownik jest powiadamiany o kontakcie czy nie.
		**/
		bool notify() const;
};

/**
	Indeksami s± altnicki (wy¶wietlane pseudonimy) konwertowane tak,
	aby wszystkie litery by³y ma³e.
	Dziêki temu nie mog± istnieæ dwa kontakty
	o tych samych pseudonimach wy¶wietlanych nawet, je¶li
	ró¿ni± siê wielko¶ciami liter.
	Pole Altnick w UserListElement zawiera natomiast
	oryginalny pseudonim wy¶wietlany.
	
	\warning U¿ywanie metod klasy QMap nie jest zalecane,
	trzeba wtedy pamiêtaæ, ¿e jako klucza u¿ywamy pseudonimów wy¶wietlanych
	odpowiednio skonwertowanych, ¿e zmiany powinny generowaæ
	odpowiednie sygna³y, itd. Du¿o lepiej u¿ywaæ specjalnie
	przygotowanych metod klasy UserList.
	\class UserList
	\brief Klasa reprezentuj±ca listê kontaktów.
	
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
		/**
			\fn UserList()
			Konstruktor standardowy.
		**/
		UserList();

		/**
			\fn UserList(const UserList& source)
			Konstruktor kopiuj±cy.
			\param source lista kontaktów, któr± nale¿y skopiowaæ.
		**/
		UserList(const UserList& source);
		~UserList();

		/**
			\fn UserList& operator=(const UserList& userlist)
			Operator kopiuj±cy.
			\param userlist lista kontaktów, któr± nale¿y skopiowaæ.
		**/
		UserList& operator=(const UserList& userlist);

		/**
			\fn UserListElement& byUin(UinType uin)
			Wyszukuje kontakt z numerem \a uin.
			\param uin numer UIN wg. którego nastêpuje wyszukiwanie.
			\return referencjê do obiektu reprezentuj±cego kontakt o podanym UIN'ie.
		**/
		UserListElement& byUin(UinType uin);
		
		/**
			\fn UserListElement& byNick(const QString& nickname);
			Wyszukuje kontakt po pseudonimie. Ma³e i
			du¿e litery nie maj± znaczenia.
			\param nickname pseudonim wg. którego nastêpuje wyszukiwanie.
			\return referencjê do obiektu reprezentuj±cego kontakt o podanym pseudonimie.
		**/
		UserListElement& byNick(const QString& nickname);

		/**
			\fn UserListElement& byAltNick(const QString& altnick)
			Wyszukuje kontakt po wy¶wietlanym pseudonimie. Ma³e i
			du¿e litery nie maj± znaczenia.
			\param altnick wy¶wietlany pseudonim wg. którego nastêpuje wyszukiwanie.
			\return referencjê do obiektu reprezentuj±cego kontakt o podanym pseudonimie wy¶wietlanym.
		**/
		UserListElement& byAltNick(const QString& altnick);

		/**
			\fn UserListElement byUinValue(UinType uin)
			Wyszukuje kontakt po numerze \a uin.
			\param uin UIN wg. którego nastêpuje wyszukiwanie.
			\return obiekt reprezentuj±cy kontakt z podanym UIN'em. Jezeli nie znajdzie, zwracany jest UserListElement zawieraj±cy tylko UIN i pseudonim wy¶wietlany (identyczny z numerem UIN).
		**/
		UserListElement byUinValue(UinType uin);

		/**
			\fn bool containsUin(UinType uin) const
			Sprawdza czy istnieje kontakt o danym numerze uin.
			\param uin sprawdzany UIN.
		**/
		bool containsUin(UinType uin) const;

		/**
			\fn bool containsAltNick(const QString& altnick) const;
			Sprawdza czy istnieje kontakt o danym pseudonimie wy¶wietlanym.
			Ma³e i du¿e litery nie maj± znaczenia.
			\param altnick Sprawdzany pseudonim wy¶wietlany.
		**/
		bool containsAltNick(const QString& altnick) const;

	public slots:
		/**
			\fn void addUser(const UserListElement& ule)
			Dodaje do listy kontakt o podanych danych.
			Generuje nastêpnie sygna³y userDataChanged()
			i modified().
			\param ule obiekt reprezentuj±cy kontakt, który chcemy dodaæ.
		**/
		void addUser(const UserListElement& ule);

		/**
			\fn void addAnonymous(UinType uin)
			Dodaje do listy "anonimowy" kontakt o podanym
			numerze UIN. Numer ten wykorzystywany jest jako
			pseudonim wy¶wietlany. Wywo³uje metodê addUser(), wiêc
			generowane s± sygna³y userDataChanged() i modified().
			\param uin UIN kontaktu, który chcemy dodaæ.
		**/
		void addAnonymous(UinType uin);

		/**
			\fn void removeUser(const QString& altnick)
			Usuwa kontakt o podanym pseudonimie wy¶wietlanym. Ma³e i
			du¿e litery nie maj± znaczenia.
			Tu¿ przed usuniêciem kontaktu z listy generuje
			sygna³ userDataChanged(). Po usuniêciu generowany
			jest sygna³ modified().
			\param altnick pseudonim wy¶wietlany kontaktu, który chcemy usun±æ.
		**/
		void removeUser(const QString& altnick);

		/**
			\fn void changeUserInfo(const QString& old_altnick, const UserListElement& new_data)
			Zmienia dane kontaktu o podanym pseudonimie wy¶wietlanym. Ma³e i
			du¿e litery w zmiennej \a old_altnick nie maj± znaczenia.
			Po wykonaniu zmian wymusza ich uwzglêdnienie przez
			wszystkie obiekty klasy UserBox wywo³uj±c metodê
			UserBox::all_renameUser() i UserBox::all_refresh().
			Na koniec generuje sygna³y userDataChanged() i modified().
			\param old_altnick dotychczasowy pseudonim wy¶wietlany kontaktu.
			\param new_data obiekt reprezentuj±cy nowe dane kontaktu.
		**/
		void changeUserInfo(const QString& old_altnick, const UserListElement& new_data);

		/**
			\fn bool writeToFile(QString filename = "")
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
			\param filename plik do którego zostanie zapisana lista.
			\return FALSE, gdy wyst±pi³y b³êdy, lub TRUE w przeciwnym wypadku.
			
			TODO: To gdzie zapisujemy dodatkowe atrybutami te¿
			powinno byæ chyba podawane jako argument? Inaczej
			je¶li chcemy zapisaæ listê gdzie¶ indziej to nadpisz±
			nam siê atrybuty :/
		**/
		bool writeToFile(QString filename = "");

		/**
			\fn bool readFromFile()
			Wczytuje listê kontaktów z plików "userlist"
			i "userattribs" z katalogu z ustawieniami.
			Metoda ta jest przeznaczona do przywracania
			danych zapisanych za pomoc± writeToFile().
			\return FALSE, gdy wyst±pi³y b³êdy, lub TRUE w przeciwnym wypadku.
		**/
		bool readFromFile();

		/**
			\fn void merge(const UserList &userlist)
			Scala listê kontaktów z podan± list± kontaktów \a userlist.
			\param userlist lista kontaktów z któr± nale¿y scaliæ
		**/
		void merge(const UserList &userlist);

	signals:
		/**
			\fn void modified()
			Sygna³ generowany po zakoñczeniu zmian w li¶cie
			kontaktów.
		**/
		void modified();

		/**
			\fn void dnsNameReady(UinType)
			Sygna³ generowany, gdy adres IP kontaktu o danym UIN'ie zostanie przet³umaczony
			na nazwê domeny.
			\param uin UIN kontaktu, dla którego przygotowano nazwê domeny.
		**/
		void dnsNameReady(UinType uin);

		/**
			\fn void userDataChanged(const UserListElement* const oldData, const UserListElement* const newData, bool massively = false)
			Sygna³ jest generowany je¶li cokolwiek zmieni³o
			siê w danych którego¶ z kontaktów na li¶cie
			(wtedy przekazywane s± argumenty \a oldData i \a newData).
			Zmiana mo¿e nast±piæ przez bezpo¶rednie u¿ycie której¶
			z metod klasy UserListElement.
			Sygna³ przekazuje równie¿ informacjê o tym, ¿e nowy
			kontakt pojawi³ siê na li¶cie,
			lub ¿e zosta³ z listy usuniêty.
			Je¿eli ta zmiana jest czê¶ci± czego¶ wiêkszego, to \a massively == true
			i emitowany jest sygna³ UserList::allNewContacts.
			
			\param oldData obiekt reprezentuj±cy stare dane kontaktu. Je¶li ten parametr ma warto¶æ NULL,
				to znaczy, ¿e kontakt w³a¶nie zosta³ dodany do listy.
			\param newData obiekt reprezentuj±cy nowe dane kontaktu. Je¶li ten parametr ma warto¶æ NULL,
				to znaczy, ¿e kontakt zosta³ w³a¶nie usuniêty z listy.
		**/
		void userDataChanged(const UserListElement* const oldData, const UserListElement* const newData, bool massively = false);

		/**
			\fn void allNewContacts(const UserList &ulist)
			Sygna³ jest emitowany, kiedy powstaje zupe³nie nowa lista kontaktów.
		**/
		void allNewContacts(const UserList &ulist);
};

extern UserList userlist;

#endif

