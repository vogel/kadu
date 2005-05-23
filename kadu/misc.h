#ifndef KADU_MISC_H
#define KADU_MISC_H

#include <qcstring.h>
#include <qdatetime.h>
#include <qdialog.h>
#include <qfiledialog.h>
#include <qfont.h>
#include <qfontinfo.h>
#include <qiconset.h>
#include <qimage.h>
#include <qlabel.h>
#include <qmap.h>
#include <qpair.h>
#include <qpixmap.h>
#include <qstringlist.h>
#include <qsocket.h>
#include <qstring.h>
#include <qtextbrowser.h>
#include <qtooltip.h>
#include <qvaluelist.h>
#include <qvariant.h>

#include <inttypes.h>

#include "config_file.h"
#include "gadu.h"

class QComboBox;
class QLineEdit;
class QMenuData;

/**
	\fn QString ggPath(const QString &subpath="")
	Zmienia ¶cie¿kê relatywn± do katalogu z ustawieniami gg
	na ¶cie¿kê bezwzgledn± uwzglêdniaj±c zmienne ¶rodowiskowe
	\c $HOME i \c $CONFIG_DIR.
	Funkcja ta jest przydatna, gdy chcemy siê odwo³aæ do jakiego¶
	pliku/katalogu znajduj±cego siê w katalogu ustawieñ aktualnej
	sesji Kadu.
	\param subpath ¶cie¿ka relatywna do katalogu z ustawieniami gg
	\return ¶cie¿kê bezwzglêdn± do pliku.
	
	<b>Przyk³ad:</b>
	\code
	MessageBox::msg(ggPath("kadu.conf"));
	\endcode
	Wy¶wietlone zostanie okienko z pe³n± ¶cie¿k± do pliku konfiguracyjnego Kadu.
**/
QString ggPath(const QString &subpath="");

/**
	\fn QString dataPath(const QString &f = "", const char *argv0 = 0)
	Zwraca bezwzglêdn± ¶cie¿kê do danego pliku.
	Je¿eli drugi parametr jest ró¿ny od 0, to funkcja próbuje najpierw ustaliæ
	¶cie¿kê na podstawie \arg argv0, które ma byæ równe argv[0], oraz zmiennej PATH.
	\param f nazwa pliku (¶cie¿ka relatywna do katalogu z danymi Kadu) do którego
			ma zostaæ zwrócona pe³na, bezwzglêdna ¶cie¿ka.
	\param argv0 nazwa uruchomionego programu. Mo¿e to byæ zarówno '<i>kadu</i>',
			jak i '<i>kadu-kopia2</i>'. Domy¶lnie jest równy 0.
	\return ¶cie¿kê bezwzglêdn± do pliku.
**/
QString dataPath(const QString &f = "", const char *argv0 = 0);

class UserListElement;

/**
	\fn QString cp2unicode(const unsigned char *buf)
	Podmienia w danym ³añcuchu znaki z kodowania CP1250
	na kodowanie Unicode.
	\param buf ³añcuch znaków do przekodowania
	\return przekodowany ³añcuch.
**/
QString cp2unicode(const unsigned char *);

/**
	\fn QCString unicode2cp(const QString &buf)
	Podmienia w danym ³añcuchu znaki z kodowania Unicode
	na kodowanie CP1250.
	\param buf ³añcuch znaków do przekodowania
	\return przekodowany ³añcuch.
**/
QCString unicode2cp(const QString &);

/**
	\fn QString latin2unicode(const unsigned char *buf)
	Podmienia w danym ³añcuchu znaki z kodowania Latin2
	na kodowanie Unicode.
	\param buf ³añcuch znaków do przekodowania
	\return przekodowany ³añcuch.
**/
QString latin2unicode(const unsigned char *);

/**
	\fn QCString unicode2latin(const QString &buf)
	Podmienia w danym ³añcuchu znaki z kodowania Latin2
	na kodowanie Unicode.
	\param buf ³añcuch znaków do przekodowania
	\return przekodowany ³añcuch.
**/
QCString unicode2latin(const QString &buf);

/**
	\fn QString unicode2std(const QString &buf)
	Podmienia w danym ³añcuchu polskie znaki z kodowania Unicode
	na ich odpowiedniki w kodowaniu ISO8859-2, czyli zmienia
	\a ñ na \a n, \a ó na \a o, \a ¶ na s, itd.
	\param buf ³añcuch znaków do przekodowania
	\return przekodowany ³añcuch.
**/
QString unicode2std(const QString &);

/**
	\fn QString unicodeUrl2latinUrl(const QString &buf)
	Zamienia kodowanie polskich znaków z utf-8 na kodowanie latin-2
	przy pomocy QUrl::encode.
	\param buf ³añcuch znaków do przekodowania
	\return przekodowany ³añcuch.
**/
QString unicodeUrl2latinUrl(const QString &buf);

/**
	\fn QString unicode2latinUrl(const QString &buf)
	Zamienia polskie znaki na format latin2 "URL" (czyli do postaci %XY,
	akceptowanej przez adresy URL).
	\param buf ³añcuch znaków do przekodowania
	\return przekodowany ³añcuch.
**/
QString unicode2latinUrl(const QString &buf);

/**
	\fn QString printDateTime(const QDateTime &datetime)
	Formatuje datê i czas do postaci czytelnej cz³owiekowi,
	czyli <tt>HH:MM:SS (DD.MM.YYYY)</tt>.
	\param datetime konkretna data, któr± nale¿y sformatowaæ
	\return sformatowan± datê.
**/
QString printDateTime(const QDateTime &datetime);

/**
	\fn QString timestamp(time_t customtime = 0)
	Formatuje stempel czasowy w postaci 2 dat: aktualna oraz podana.
	Ma on postaæ: <tt>HH:MM:SS (DD.MM.YYYY) /S HH:MM:SS (DD.MM.YYYY)</tt>,
	gdzie pierwsza data oznacza aktualn± datê pobieran± podczas wykonywania tej funkcji,
	a druga data to data podana jako argument i sformatowatowana przez funkcjê unicode2latinUrl.
	Znacznik \c /S jest zwyk³ym ³añcuchem '/S'.
	\param customtime czas podany w formie sekund od pocz±tku roku 1970 (tzw. '<I>unixtime</i>'),
			który jest sformatowany i zwrócony jako druga data.
	\return sformatowany stempel czasowy
**/
QString timestamp(time_t = 0);

/**
	\fn QDateTime currentDateTime()
	Funkcja s³u¿±ca ³atwemu pobieraniu aktualnej daty.
	\return aktualn± datê.
**/
QDateTime currentDateTime();

/**
	\fn QString pwHash(const QString &tekst)
	Funkcja hashuj±ca zadany ³añcuch tak, aby nie by³ od ³atwy do odczytania
	dla cz³owieka, bez uprzedniego potraktowania go w odpowiedni sposób.
	\param tekst ³añcuch który ma zostaæ zahashowany
	\return zahashowany ³añcuch
**/
QString pwHash(const QString &tekst);

/**
	\fn QString translateLanguage(const QApplication *application, const QString &locale, const bool l2n)
	T³umaczy nazwy skrótowe jêzyków na ich pe³ne nazwy i vice versa, czyli przyk³adowo
	\c pl przet³umaczy na \c Polski, a \c Polski przet³umaczy na \c pl.
	Je¶li podane wyra¿enie nie mo¿e zostaæ odnalezione w tablicy ty³umaczeñ, wtedy zwracany jest
	odpowiednia nazwa dla jêzyka angielskiego.
	\param application obiekt aplikacji Qt, który ma przet³umaczyæ. Zwykle jest to \c qApp
	\param locale wyra¿enie, które ma zostaæ odpowiednio przet³umaczone. Mo¿e to byæ:
			\c pl, \c en, \c de, \c it, ale równie¿:
			\c Polski, \c Angielski, \c Niemiecki, \c W³oski
	\param l2n okre¶la, czy parametr \a locale jest wersj± 2-literow± (\c true), czy pe³n± nazw± (\c false)
	\return s³owo odpowiadaj±ce podanemu argumentowi \a locale.
**/
QString translateLanguage(const QApplication *application, const QString &locale, const bool l2n);

/**
	\fn void openWebBrowser(const QString &link)
	Otwiera domy¶ln± przegl±darkê i wybiera w niej podany adres.
	\param link adres URL, który ma byæ otwarty w przegl±darce
**/
void openWebBrowser(const QString &link);

/**
	\fn QString formatGGMessage(const QString &msg, int formats_length, void *formats, UinType sender)
	Formatuje wiadomo¶æ protoko³u GG do postaci HTML (pogróbienie, kursywa, podkre¶lenie, kolor).
	\param msg tre¶æ wiadomo¶ci
	\param formats_length d³ugo¶æ parametrów formatuj±cych (szczegó³y w dokumentacji libgadu)
	\param formats wska¼nik do struktur formatuj±cych (szczegó³y w dokumentacji libgadu)
	\param sender UIN kontaktu, który ma byæ u¿yty do sformatowania wiadomo¶ci (przychodz±cej)
	\return gotow± wiadomo¶æ sformatowan± w HTMLu.
**/
QString formatGGMessage(const QString &msg, int formats_length, void *formats, UinType sender);

/**
	\fn QString unformatGGMessage(const QString &msg, int &formats_length, void *&formats)
	Formatuje wiadomo¶æ z postaci HTML do formatu akceptowanego przez protokó³ GG.
	\param msg tre¶æ wiadomo¶ci ze znacznikami HTML
	\param formats_length zmienna typu integer, do której zapisana zostanie d³ugo¶æ
			parametrów formatuj±cych (szczegó³y w dokumentacji libgadu)
	\param formats wska¼nik, pod którym zapisane zostan± struktury formatuj±ce
			wiadomo¶æ (szczegó³y w dokumentacji libgadu)
	\return tre¶æ wiadomo¶ci z usuniêtymi znacznikami HTML (s± one teraz zakodowane
			w \a formats i \a formats_length ).
**/
QString unformatGGMessage(const QString &msg, int &formats_length, void *&formats);

/**
	\fn QString parse(const QString &s, const UserListElement &ule, bool escape = true)
	Podmienia odpowiednie znaczniki w danym ³añcuchu wed³ug danych podanego kontaktu.
	Dozwolone s± nastêpuj±ce znaczniki podstawowe:
	\arg \c %%s status
	\arg \c %%t status w orginalnej (nie przet³umaczonej) formie.
	\arg \c %%d opis (lub pusty ³añcuch je¶li opisu nie ma)
	\arg \c %%i adres IP (je¶li znany, inaczej pusty ³añcuch)
	\arg \c %%v adres DNS (je¶li znany, inaczej pusty ³añcuch)
	\arg \c %%o je¶li kontakt ma u¿ytkownika Kadu na li¶cie, to ten znacznik podmieniany jest na pojedyncz± spacjê
	\arg \c %%p port (je¶li znany, inaczej pusty ³añcuch)
	\arg \c %%u numer UIN
	\arg \c %%n pseudonim
	\arg \c %%a nazwa wy¶wietlana
	\arg \c %%f imiê
	\arg \c %%r nazwisko
	\arg \c %%m numer telefonu komórkowego
	\arg \c %%g grupa (lub kilka grup oddzielonych przecinkiem)
	\arg \c %%e adres e-mail
	\arg \c %%x maksymalny rozmiar obrazka w KB
	\arg \c %%% pojedynczy znak '%'

	Ponadto mo¿na stosowaæ nawiasy \c [] i \c {}, oraz apostrofy: \c `'.

	Nawiasy kwadratowe pozwalaj± na u¿ywanie swego rodzaju instrukcji warunkowej,
	która sprawdza czy wszystkie (bez wyj±tku) znaczniki zawarte pomiêdzy tymi
	nawiasami zwróci³y warto¶æ niepust±. Je¶li tak, to w miejsce nawiasów wstawiany
	jest po prostu ³añcuch zawarty miêdzy nimi (z podmienionymi ju¿ znacznikami),
	je¶li natomiast który¶ ze znaczników zwróci³ warto¶æ pust± (np. %o, co oznacza³o
	by ¿e kontakt nie ma u¿ytkownika Kadu na li¶cie), to w miejsce nawiasów wstawiana
	jest warto¶æ pusta.

	Nawiasy klamrowe natomiast sprawdzaj±, czy nazwa pliku - podana jako pierwszy wyraz zawarty
	w tych nawiasach - istnieje. Je¶li tak, to w miejsce tych nawiasów wstawiany jest ca³y ³añcuch
	miêdzy nimi zawarty, za wyj±tkiem tej pierwszej nazwy pliku (i odstêpu po niej). Je¶li natomiast
	plik nie istnieje, to wstawiany jest pusty ³añcuch.

	Wreszcie apostrofy ` i '. Dziêki nim mo¿na wykonaæ polecenie w pow³oce systemu, a to co zwróci
	ta komenda, zostanie wstawione w miejsce ca³ego wyra¿enia miêdzy apostrofami (³±cznie z nimi samymi).

	\param s ³añcuch ze znacznikami do podmiany
	\param ule obiekt reprezentuj±cy kontakt, którego dane bêd± podstawiane pod znaczniki
	\param escape warto¶æ logiczna decyduj±ca o tym, czy znaczniki HTML maj± zostaæ zinterpretowane (\c false)
			czy te¿ potraktowaæ je jako zwyk³y tekst (\c true).
	\return ³añcuch z podstawionymi danymi zamiast znaczników
**/
QString parse(const QString &s, const UserListElement &ule, bool escape = true);

/**
	\fn void stringHeapSort(QStringList &c)
	\internal
	Funkcja sortuj±ca odpowiednio listê ³añcuchów.
	\todo Jakie to dok³adnie sortowanie?
**/
void stringHeapSort(QStringList &c);

/**
	\fn QStringList toStringList(const QString &e1, const QString &e2=QString(), const QString &e3=QString(), const QString &e4=QString(), const QString &e5=QString())
	£±czy dowoln± liczbê (nie wiêksz± ni¿ 5) ³añcuchów QString w jedn± listê QStringList.
	\param e1 pierwszy ³añcuch, wymagany
	\param e2 drugi ³añcuch opcjonalny
	\param e3 trzeci ³añcuch opcjonalny
	\param e4 czwarty ³añcuch opcjonalny
	\param e5 pi±ty ³añcuch opcjonalny
	\return obiekt reprezentuj±cy listê podanych ³añcuchów
**/
QStringList toStringList(const QString &e1, const QString &e2=QString(), const QString &e3=QString(), const QString &e4=QString(), const QString &e5=QString());

/**
	\fn void saveGeometry(const QWidget *w, const QString &section, const QString &name)
	Zapisuje wymiary i po³o¿enie wskazanego okna do danej sekcji konfiguracyjnej,
	do klucza o podanej nazwie.
	\param w wska¼nik do obiektu, którego wymiary i po³o¿enie maj± zostaæ zapisane
	\param section sekcja pliku konfiguracyjnego, w której ma siê znale¼æ wpis
	\param name nazwa klucza, w jakim maj± byæ zapisane te dane
**/
void saveGeometry(const QWidget *w, const QString &section, const QString &name);

/**
	\fn void loadGeometry(QWidget *w, const QString &section, const QString &name, int defaultX, int defaultY, int defaultWidth, int defaultHeight)
	£aduje wymiary i po³o¿enie z podanej sekcji i klucza i aplikuje je do podanego okna.
	\param w wska¼nik do obiektu, do którego odczytane wymiary i po³o¿enie maj± zostaæ zastosowane
	\param section sekcja pliku konfiguracyjnego, z którego dane bêd± czytane
	\param name nazwa klucza, z którego dane bêd± czytane
	\param defaultX domy¶lna warto¶æ pozycji lewego górnego rogu okna w poziomie (gdy brakuje wpisu w pliku konfiguracyjnym)
	\param defaultY domy¶lna warto¶æ pozycji lewego górnego rogu okna w pionie (gdy brakuje wpisu w pliku konfiguracyjnym)
	\param defaultWidth domy¶lna warto¶æ szeroko¶ci okna (gdy brakuje wpisu w pliku konfiguracyjnym)
	\param defaultHeight domy¶lna warto¶æ wysoko¶ci okna (gdy brakuje wpisu w pliku konfiguracyjnym)
**/
void loadGeometry(QWidget *w, const QString &section, const QString &name, int defaultX, int defaultY, int defaultWidth, int defaultHeight);

/**
	\fn QString toPlainText(const QString &text)
	Usuwa znaki nowego wiersza, oraz tagi HTMLa (wszystko co da siê dopasowaæ jako wyra¿enie regularne: \c <.*> ).
	\param text ³añcuch do przetworzenia
	\return ³añcuch z usuniêtymi odpowiednimi znakami.
**/
QString toPlainText(const QString &text);

/**
	\var extern QFont *defaultFont
	Domy¶lna czcionka Kadu.
**/
extern QFont *defaultFont;

/**
	\var extern QFontInfo *defaultFontInfo
	Wska¼nik do obiekt zawieraj±cego informacje dotycz±ce domy¶lnej czcionki Kadu
**/
extern QFontInfo *defaultFontInfo;

/**
	\var extern QTextCodec *codec_cp1250
	Wska¼nik do kodeka CP1250.
**/
extern QTextCodec *codec_cp1250;

/**
	\var extern QTextCodec *codec_latin2
	Wska¼nik do kodeka Latin2.
**/
extern QTextCodec *codec_latin2;

/**
	Klasa reprezentuj±ca okno wybierania opisów do statusów.
	\class ChooseDescription
	\brief Okno wyboru opisu
**/
class ChooseDescription : public QDialog
{
	Q_OBJECT
	public:
		/**
			\fn ChooseDescription(int nr, QWidget * parent=0, const char * name=0)
			Konstruktor tworz±cy okno, wczytuj±cy ostatnio ustawiane opisy,
			oraz ustawiaj±cy odpowiedni± dla statusu ikonê w tym oknie.
			\param nr numer statusu, gdzie:
				\arg \c 1 oznacza status <tt>Dostêpny z opisem</tt>
				\arg \c 3 oznacza status <tt>Zajêty z opisem</tt>
				\arg \c 5 oznacza status <tt>Ukryty z opisem</tt>
				\arg \c 7 oznacza status <tt>Niedostêpny z opisem</tt>
				\arg <tt>Ka¿dy inny</tt> oznacza status <tt>Niedostêpny z opisem</tt>
			\param parent rodzic kontrolki (domy¶lnie: 0)
			\param name nazwa kontrolki (domy¶lnie: 0)
		**/
		ChooseDescription ( int nr, QWidget * parent=0, const char * name=0);

		/**
			\fn virtual ~ChooseDescription()
			Domy¶lny destruktor. Ustawia opis (je¶li klikniêto OK).
		**/
		virtual ~ChooseDescription();

		/**
			\fn void getDescription(QString &dest)
			Funkcja s³u¿±ca do odczytu tre¶ci z pola edycji opisu.
			\param dest obiekt klasy QString, pod którym zapisana zostanie tre¶æ
			\warning funkcja nie zwraca ³añcucha, a zapisuje go do podanego parametru
		**/
		void getDescription(QString &);

	private:
		QComboBox *desc;
		QLabel *l_yetlen;

	private slots:
		void okbtnPressed();
		void cancelbtnPressed();
		void updateYetLen(const QString&);
};

/**
	Klasa bardzo pomocna, gdy chcemy, aby Kadu udawa³o przez chwilê przegl±darkê WWW.
	Pozwala na stosunkowo ³atwy odczyt danych ze stron, ale tak¿e bardziej zaawansowane
	operacje, jak logowanie na stronach metod± POST, czy te¿ poprzez ciasteczka.
	\class HttpClient
	\brief Klient stron WWW
**/
class HttpClient : public QObject
{
	Q_OBJECT

	private:
		QSocket Socket;
		QString Host;
		QString Referer;
		QString Path;
		QByteArray Data;
		QByteArray PostData;
		int StatusCode;
		bool HeaderParsed;

		unsigned int ContentLength;
		bool ContentLengthNotFound;

		QMap<QString, QString> Cookies;

	private slots:
		void onConnected();
		void onReadyRead();
		void onConnectionClosed();

	public slots:
		/**
			\fn void setHost(const QString &host)
			Ustawia adres serwera, z którego bêdziemy czytaæ stronê.
			\param host adres hosta w postaci ³añcucha
		**/
		void setHost(const QString &host);

		/**
			\fn void get(const QString &path)
			Wysy³a zapytanie GET do ustawionego wcze¶niej serwera.
			\param path ¶cie¿ka do dokumentu, który chcemy pobraæ z serwera

			<b>Przyk³ad:</b>
			\code
			http_client->get("/galeria/index.html");
			\endcode
		**/
		void get(const QString &path);

		/**
			\fn void post(const QString &path, const QByteArray &data)
			Wysy³a do serwera zapytanie typu POST, dodaj±c do niego
			odpowiednie dane.
			\param path ¶cie¿ka do dokumentu, który chcemy pobraæ z serwera
			\param data parametry, które s± przekazane do serwera - zwykle zawieraj±
					warto¶ci zmiennych z form na uprzednio pobranej stronie.
		**/
		void post(const QString &path, const QByteArray &data);

		/**
			\overload void post(const QString &path, const QString &data)
		**/
		void post(const QString &path, const QString &data);

	public:
		/**
			\fn HttpClient()
			Standardowy konstruktor.
		**/
		HttpClient();

		/**
			\fn int status() const
			\return kod aktualnego statu klienta WWW, zgodny z protoko³em HTTP (302, 500, itd).
		**/
		int status() const;

		/**
			\fn const QByteArray &data() const
			Funkcja daj±ca dostêp do danych reprezentuj±cych kod pobranej strony.
			\return tre¶æ pobranej strony.
		**/
		const QByteArray &data() const;

		/**
			\fn static QString encode(const QString &text)
			Zamienia znaki nieakceptowalne w adresie strony na ich odpowiedniki w formie %XY.
			\param text ³añcuch reprezentuj±cy adres strony, zawieraj±cy spacje, itp
			\return sformatowany ³añcuch.
		**/
		static QString encode(const QString &text);

		/**
			\fn const QString &cookie(const QString &name) const
			Funkcja daj±ca dostêp do danych zawartych w tzw. ciasteczkach.
			\param name nazwa ciasteczka
			\return warto¶æ dla danego ciasteczka.
		**/
		const QString &cookie(const QString &name) const;

		/**
			\fn const QString &cookie(const QString &name) const
			Funkcja daj±ca dostêp do danych zawartych w tzw. ciasteczkach.
			\return obiekt klasy QMap zawieraj±cy sparowane nazwy ciasteczek z ich warto¶ciami.
		**/
		const QMap<QString, QString> &cookies() const;

		/**
			\fn void setCookie(const QString &name, const QString &value)
			Ustawia warto¶æ danego ciasteczka.
			\param name nazwa ciasteczka
			\param value warto¶æ dla danego ciasteczka
		**/
		void setCookie(const QString &name, const QString &value);

	signals:
		/**
			\fn void finished()
			Sygna³ emitowany, gdy zapytanie zostanie sfinalizowane.
		**/
		void finished();

		/**
			\fn void redirected(QString link)
			Sygna³ emitowany, gdy klient HTTP zostanie przekierowany przez serwer hosta
			na inny adres.
			\param link adres strony na jaki klient zosta³ przekierowany
		**/
		void redirected(QString link);

		/**
			\fn void error()
			Sygna³ emitowany, gdy wyst±pi³ b³±d podczas próby sfninaliowania zapytania.
		**/
		void error();
};

/**
	Klasa reprezentuj±ca dokument HTML. Przechowuje
	listê elementów. Element mo¿e byæ tagiem HTML
	albo kawa³kiem tekstu.
	\class HtmlDocument
	\brief Dokument HTML
**/
class HtmlDocument
{
	private:
		/**
			Struktura reprezentuj±ca pojedynczy element dokumentu.
			\struct Element
			\brief Element dokumentu
		**/
		struct Element
		{
			QString text; /*!< tre¶æ elementu */
			bool tag; /*!< oznacza, czy element jest takiem HTML, czy zwyk³ym tekstem */
		};
		QValueList<Element> Elements;
		void addElement(Element e);

	public:
		/**
			\fn static void escapeText(QString& text)
			Zamienia znaki interpretowane przez przegl±darki HTML (tj. \c < , \c > , itd.)
			na ich odpowiedniki zakodowane w HTML (tj. \c &lt; , \c &gt; , itd.).
			\param text obiekt klasy QString zawieraj±cy znaki do podmiany
			\warning funkcja nie zwraca ³añcucha z podmienionymi znakami,
					a zapisuje go od od razu do podanego obiektu, zmieniaj±c jego zawarto¶æ.
		**/
		static void escapeText(QString& text);

		/**
			\fn static void unescapeText(QString& text)
			Zamienia znaki zakodowane w HTML (tj. \c &lt; , \c &gt; , itd.)
			na ich odpowiedniki interpretowane przez przegl±darki HTML (tj. \c < , \c > , itd.).
			\param text obiekt klasy QString zawieraj±cy znaki do podmiany
			\warning funkcja nie zwraca ³añcucha z podmienionymi znakami,
					a zapisuje go od od razu do podanego obiektu, zmieniaj±c jego zawarto¶æ.
		**/
		static void unescapeText(QString& text);

		/**
			\fn void addTag(const QString &text)
			Dodaje podany tekst do listy elementów jako
			tag HTML.
			\param text tekst do dodania
		**/
		void addTag(const QString &text);

		/**
			\fn void addText(const QString &text)
			Dodaje podany tekst do listy elementów jako
			zwyk³y tekst.
			\param text tekst do dodania
		**/
		void addText(const QString &text);

		/**
			\fn void insertTag(const int pos,const QString &text)
			Wstawia podany tekst do listy elementów jako
			tag HTML przed innym elementem.
			\param pos indeks elementu, przed którym wstawiamy
			\param text tekst do dodania
		**/
		void insertTag(const int pos,const QString &text);

		/**
			\fn void insertText(const int pos,const QString &text)
			Wstawia podany tekst do listy elementów jako
			zwykly tekst przed innym elementem.
			\param pos indeks elementu, przed którym wstawiamy
			\param text tekst do dodania
		**/
		void insertText(const int pos,const QString &text);

		/**
			\fn void parseHtml(const QString& html)
			Parsuje podany napis zgodny ze struktur± HTML
			i otrzymane dane przypisuje do dokumentu.
			@param html napis do skonwertowania
		**/
		void parseHtml(const QString& html);

		/**
			\fn QString generateHtml() const
			Na podstawie zawarto¶ci dokumentu generuje
			napis zgodny ze struktur± HTML. Znaki specjalne
			wchodz±ce w sk³ad elementów nie bêd±cych tagami
			HTML s± escapowane.
			\return zawarto¶æ dokumentu w formacie HTML.
		**/
		QString generateHtml() const;

		/**
			\fn int countElements() const
			\return ilo¶æ elementów wchodz±cych w sk³ad
			dokumentu.
		**/
		int countElements() const;

		/**
			\fn bool isTagElement(int index) const
			Sprawdza czy element o podanym indeksie jest
			tagiem HTML czy zwyk³ym tekstem.
			\return \c true je¶li jest tagiem HTML, lub \c false gdy jest zwyk³ym tekstem.
		**/
		bool isTagElement(int index) const;

		/**
			\fn const QString &elementText(int index) const
			\return tekst elementu o podanym indeksie.
		**/
		const QString &elementText(int index) const;

		/**
			\overload QString &elementText(int index)
		**/
		QString &elementText(int index);

		/**
			\fn void setElementValue(int index,const QString& text,bool tag=false)
			Ustawia tekst i typ elementu o podanym indeksie.
			\param index indeks pozycji elementu
			\param text tre¶æ elementu
			\param tag czy element ma byæ interpretowany jako tag HTML (\c true ),
				czy jako zwyk³y tekst (\c false ). Domy¶lnie: \c false.
		**/
		void setElementValue(int index,const QString& text,bool tag=false);

		/**
			\fn void splitElement(int& index,int start,int length)
			Wydziela z elementu podany fragment tekstu.
			Element mo¿e pozostaæ jeden albo
			ulec podzieleniu na dwa lub nawet trzy elementy.
			\param index indeks pozycji elementu który bêdziemy dzieliæ
			\param start numer znaku, od którego zaczynaæ ma siê wydzielony element
			\param length liczba znaków wchodz±cych w wydzielony element, poczynaj±c od pozycji \c start
			\note Funkcja zmienia warto¶æ argumentu index, aby
			indeks wydzielonego elementu by³ aktualny.
		**/
		void splitElement(int& index,int start,int length);

		/**
			\fn void convertUrlsToHtml()
			Skanuje tekst dokumentu w poszukiwaniu adresów WWW,
			np. "www.kadu.net" i zmienia je w linki HTML.
		**/
		void convertUrlsToHtml();
};

/*
	nie u¿ywane - patrz opis w pliku .cpp

HtmlDocument GGMessageToHtmlDocument(const QString &msg, int formats_length, void *formats);
void HtmlDocumentToGGMessage(HtmlDocument &htmldoc, QString &msg,
	int &formats_length, void *&formats);
*/

/**
	Klasa reprezentuj±ca kontrolki obrazków.
	\class ImageWidget
	\brief Obrazki
**/
class ImageWidget : public QWidget
{
	Q_OBJECT

	private:
		QImage Image;

	protected:
		/**
			\fn virtual void paintEvent(QPaintEvent *e)
			Funkcja rysuj±ca t± kontrolkê.
			\param e wska¼nik do obiektu reprezentuj±cego zdarzenie rysowania.
		**/
		virtual void paintEvent(QPaintEvent *e);

	public:
		/**
			\fn ImageWidget(QWidget *parent)
			Konstruktor tworz±cy kontrolkê w innej - podanej kontrolce.
			\param parent kontrolka, w której nale¿y obsadziæ
		**/
		ImageWidget(QWidget *parent);

		/**
			\overload ImageWidget(QWidget *parent,const QByteArray &image)
			Konstruktor tworz±cy kontrolkê w innej - podanej kontrolce,
			oraz rysuj±cy od razu dany obrazek.
			\param parent kontrolka, w której nale¿y obsadziæ
			\param image dane zawieraj±ce obrazek
		**/
		ImageWidget(QWidget *parent,const QByteArray &image);

		/**
			\fn void setImage(const QByteArray &image)
			Ustawia obrazek kontrolki na zadany.
			\param image dane zawieraj±ce obrazek.
		**/
		void setImage(const QByteArray &image);

		/**
			\overload void setImage(const QPixmap &image)
		**/
		void setImage(const QPixmap &image);
};

/**
	Klasa reprezentuj±ca okienko z obrazkiem i polem edycji.
	Jest ona u¿ywana do wy¶wietlania obrazka z bramki SMS Idei,
	aby u¿ytkownik móg³ odczytaæ z niego tre¶æ i wpisaæ j± do pola edycji.
	\brief Odczytywanie tre¶ci z obrazka WWW
	\class TokenDialog
**/
class TokenDialog : public QDialog
{
	Q_OBJECT

	public:
		/**
			\fn TokenDialog(QPixmap tokenImage, QDialog *parent = 0, const char *name = 0)
			Konstruktor tworz±cy okienko wraz z gotowym obrazkiem.
			\param tokenImage obrazek do wy¶wietlenia
			\param parent rodzic kontrolki (domy¶lnie: 0)
			\param name nazwa kontrolki (domy¶lnie: 0)
		**/
		TokenDialog(QPixmap tokenImage, QDialog *parent = 0, const char *name = 0);

		/**
			\fn void getValue(QString &tokenValue)
			Pobiera tre¶æ pola edycji spod obrazka.
			\param tokenValue obiekt klasy QString, pod którym zostanie zapisana pobrana tre¶æ.
		**/
		void getValue(QString &tokenValue);

	private:
		QLineEdit *tokenedit;
};

/**
	Klasa zarz±dzaj±ca stylami wygl±du Kadu.
	\class Themes
	\brief Style Kadu
**/
class Themes : public QObject
{
	Q_OBJECT
	private:
		QStringList ThemesList;
		QStringList ThemesPaths;
		QStringList additional;
		QString ConfigName, Name, ActualTheme;
		QMap<QString, QString> entries;
		QStringList getSubDirs(const QString& path) const;
		QString fixFileName(const QString& path,const QString& fn) const;

	public:
		/**
			\fn Themes(const QString& name, const QString& configname, const char *name=0)
			Konstruktor tworz±cy grupê styli Kadu.
			\param name nazwa grupy styli (\c emoticons , \c sounds , \c icons )
			\param configname nazwa pliku konfiguracyjnego
			\param name nazwa obiektu (domy¶lnie: 0)
		**/
		Themes(const QString& name, const QString& configname, const char *name=0);

		/**
			\fn QStringList defaultKaduPathsWithThemes() const
			\return domy¶ln± listê ¶cie¿ek, w których Kadu trzyma grupy styli.
		**/
		QStringList defaultKaduPathsWithThemes() const;

		/**
			\fn const QStringList &themes() const
			\return listê styli Kadu.
		**/
		const QStringList &themes() const;

		/**
			\fn const QStringList &themes() const
			\return bierz±cy styl.
		**/
		const QString &theme() const;

		/**
			\fn const QStringList &paths() const
			\return listê ¶cie¿ek, w których Kadu trzyma grupy styli.
		**/
		const QStringList &paths() const;

		/**
			\fn const QStringList &additionalPaths() const
			\return listê dodatkowych ¶cie¿ek, w których Kadu trzyma grupy styli.
		**/
		const QStringList &additionalPaths() const;

		/**
			\fn QString themePath(const QString& theme="") const
			Pobiera ¶cie¿kê do danego stylu.
			\param theme nazwa stylu (domy¶lnie: "", co oznacza styl domy¶lny)
			\return ¶cie¿kê do katalogu podanego stylu
		**/
		QString themePath(const QString& theme="") const;

		/**
			QString getThemeEntry(const QString& name) const
			Pobiera warto¶æ danego wpisu.
			\param name nazwa klucza, dla którego nale¿y pobraæ wpis
			\return wpis z danego stylu, o podanym kluczu
			\todo opisaæ dok³adniej co to za wpisy.
		**/
		QString getThemeEntry(const QString& name) const;

	public slots:
		/**
			\fn void setTheme(const QString& theme)
			Ustawia aktualny styl.
			\param theme styl, który ma byæ ustawiony
		**/
		void setTheme(const QString& theme);

		/**
			\fn void setPaths(const QStringList& paths)
			Ustawia ¶cie¿ki dla styli.
			\param paths lista ¶cie¿ek
		**/
		void setPaths(const QStringList& paths);

	signals:
		/**
			\fn void themeChanged(const QString& theme)
			Sygna³ emitowany, gdy zmieniony zosta³ styl.
			\param theme nazwa nowego stylu
		**/
		void themeChanged(const QString& theme);

		/**
			\fn void pathsChanged(const QStringList& list)
			Sygna³ emitowany, gdy ¶cie¿ki do styli zosta³y zmienione.
			\param list lista nowych ¶cie¿ek
		**/
		void pathsChanged(const QStringList& list);
};

/**
	Klasa zarz±dzaj±ca zestawami ikon, oraz samymi ikonami.
	Istnieje globalna zmienna - icons_manager_ptr - tak wiêc tworzenie
	nowych instancji tego obiektu nie jest potrzebne.
	\class IconsManager
	\brief Menad¿er ikon
**/
class IconsManager :public Themes
{
	Q_OBJECT
	public:
		/**
			\fn IconsManager(const QString& name, const QString& configname)
			Konstruktor tworz±cy grupê styli ikon.
			\param name nazwa grupy
			\param configname nazwa pliku konfiguracyjnego
		**/
		IconsManager(const QString& name, const QString& configname);

		/**
			\fn QString iconPath(const QString &name) const
			T³umaczy nazwê ikony z zestawu na ¶cie¿kê do niej i na odwrót.
			\param name nazwa ikony z zestawu lub scie¿ka do pliku
					(je¶li zawiera znak '/' to jest interpretowana jako
					scie¿ka).
			\return pe³n± ¶cie¿kê do ikony z aktualnego zestawu,
					lub bezposrednio nazwê je¶li jest to pe³na ¶cie¿ka.
		**/
		QString iconPath(const QString &name) const;

		/**
			\fn const QPixmap & loadIcon(const QString &name)
			£aduje ikonê z aktualnego zestawu lub z podanego pliku.
			\param name nazwa ikony z zestawu lub ¶cie¿ka do pliku
					(je¶li zawiera znak '/' to jest interpretowana jako
					¶cie¿ka).
			\return obiekt klasy QPixmap reprezentuj±cy dan± ikonê.
		**/
		const QPixmap & loadIcon(const QString &name);

		/**
			\fn static void initModule()
			Inicjalizuje modu³ menad¿era ikon.
		**/
		static void initModule();

		/**
			\fn void registerMenu(QMenuData *menu)
			Rejestruje nowe menu.
			\param menu wska¼nik do obiektu danych dla menu
			\todo opisaæ dok³adniej co to za menu.
		**/
		void registerMenu(QMenuData *menu);

		/**
			\fn void registerMenu(QMenuData *menu)
			Wyrejestruje menu.
			\param menu wska¼nik do obiektu danych menu
			\todo opisaæ dok³adniej co to za menu.
		**/
		void unregisterMenu(QMenuData *menu);

		/**
			\fn void registerMenuItem(QMenuData *menu, const QString &caption, const QString &iconName)
			Rejestruje nowy element w menu.
			\param menu wska¼nik do obiektu danych menu
			\param caption tytu³ nowego elementu
			\param iconName nazwa ikony dla nowego elementu
		**/
		void registerMenuItem(QMenuData *menu, const QString &caption, const QString &iconName);

		/**
			\fn void unregisterMenuItem(QMenuData *menu, const QString &caption)
			Wyrejestrowuje element z menu.
			\param menu wska¼nik do obiektu danych menu
			\param caption tytu³ elementu
		**/
		void unregisterMenuItem(QMenuData *menu, const QString &caption);

	public slots:
		/**
			\fn void clear()
			Czy¶ci zestaw ikon.
		**/
		void clear();

		/**
			\fn void refreshMenus()
			Od¶wierza menu.
		**/
		void refreshMenus();

	private:
		QMap<QString, QPixmap> icons;
		
		QValueList<QPair<QMenuData *, QValueList<QPair<QString, QString> > > > menus;

	private slots:
		void selectedPaths(const QStringList& paths);
		void onCreateConfigDialog();
		void onApplyConfigDialog();
};

/**
	\var extern IconsManager *icons_manager_ptr
	Wska¼nik do obiektu generalnego zarz±dcy ikonek Kadu.
**/
//TODO: po wydaniu 0.4 trzeba zmieniæ nazwê na icons_manager i wywaliæ define'a
extern IconsManager *icons_manager_ptr;
/**
	\def icons_manager
	Tymczasowy skrót bezpo¶rednio do obiektu generalnego zarz±dcy ikonek Kadu,
	zamiast wska¼nika icons_manager_ptr.
**/
#define icons_manager (*icons_manager_ptr)


/**
	Klasa rozwiazujaca problem z powiadomieniem
	o utworzeniu nowej instancji danej klasy.
	Umieszczamy w klasie publiczna statyczna
	zmienna createNotifier klasy CreateNotifier,
	do ktorej mog± siê pod³±czaæ pozosta³e cze¶ci kodu.
	Przed wyj¶ciem z konstruktora wywo³ujemy metodê:
	\code createNotifier.notify(this); \endcode
	\class CreateNotifier
	\brief Powiadamianie o nowych instancjach klas
**/
class CreateNotifier : public QObject
{
	Q_OBJECT

	public:
		/**
			\fn void notify(QObject* new_object)
			Emituje sygna³ do wszystkich obiektów pod³±czonych do tego obiektu
			informuj±cy o tym, ¿e dany obiekt zosta³ utworzony. Funkcja ta powinna
			byæ wywo³ana przez nowotworzony obiekt, w jego konstruktorze.
			\param new_object wska¼nik do obiekty, który zosta³ utworzony (zwykle podaje siê \c this )
		**/
		void notify(QObject* new_object);

	signals:
		/**
			\fn void objectCreated(QObject* new_object)
			Sygna³ emitowany, gdy jaki¶ obiekt wywo³a metodê notify() (zwykle z parametrem \c this ).
			\param new_object wska¼nik do nowoutworzonego obiektu
		**/
		void objectCreated(QObject* new_object);
};

/**
	Klasa zarz±dzaj±ca obrazkami przesy³anymi przez protokó³ GG.
	\class GaduImagesManager
	\brief Zarz±dca obrazków protoko³u GG
**/
class GaduImagesManager
{
	private:
		struct ImageToSend
		{
			uint32_t size;
			uint32_t crc32;
			QString file_name;
			QDateTime lastSent;
			char *data;
		};
		QMap<QPair<uint32_t, uint32_t>, ImageToSend> ImagesToSend;//size,crc32 -> 

		struct SavedImage
		{
			uint32_t size;
			uint32_t crc32;
			QString file_name;
		};
		QValueList<SavedImage> SavedImages;

	public:
		/**
			\fn static void setBackgroundsForAnimatedImages(HtmlDocument &doc, const QColor &col)
			Ustawia kolor t³a dla animowanych obrazków w danym dokumencie.
			\param doc dokument, w którym kolory zostan± ustawione
			\param col kolor t³a
		**/
		static void setBackgroundsForAnimatedImages(HtmlDocument &doc, const QColor &col);

		/**
			\fn static QString loadingImageHtml(UinType uin, uint32_t size, uint32_t crc32)
			Generuje odpowiedni kod HTML, który nale¿y wstawiæ do okna rozmowy, aby pokazaæ,
			¿e obrazek jest w trakcie ³adowania (przesy³ania), oraz ustawia
			odpowiednie parametry znacznika &lt;img&gt; tak, aby mo¿na by³o potem ³atwo
			zidentyfikowaæ, jaki obrazek mia³ byæ wstawiony w to miejsce.
			W praktyce daje nam znan± klepsydrê.
			\param uin UIN kontaktu wysy³aj±cego obrazek
			\param size rozmiar obrazka
			\param crc32 suma kontrolna obrazka
			\return ³añcuch stanowi±cy kod HTML
		**/
		static QString loadingImageHtml(UinType uin, uint32_t size, uint32_t crc32);

		/**
			\fn static QString imageHtml(const QString& file_name)
			Generuje kod HTML, który nale¿y wstawiæ do okna rozmowy,
			aby pokazaæ w nim obrazek.
			\param file_name nazwa pliku obrazka
			\return ³añcuch stanowi±cy kod HTML
		**/
		static QString imageHtml(const QString& file_name);

		/**
			\fn void addImageToSend(const QString& file_name, uint32_t& size, uint32_t& crc32)
			Dodaje obrazek do listy obrazków, które maj± byæ wys³ane z kolejn± wiadomo¶ci±.
			\param file_name nazwa pliku obrazka
			\param size rozmiar obrazka
			\param crc32 suma kontrolna obrazka
		**/
		void addImageToSend(const QString& file_name, uint32_t& size, uint32_t& crc32);

		/**
			\fn void sendImage(UinType uin, uint32_t size, uint32_t crc32)
			Wysy³a jeden z obrazków znajduj±cych siê na uprzednio przygotowanej li¶cie.
			\param uin numer UIN do które ma byæ wys³any obrazek
			\param size rozmiar obrazka, który chcemy wys³aæ
			\param crc32 suma kontrolna obrazka, który chcemy wys³aæ
		**/
		void sendImage(UinType uin, uint32_t size, uint32_t crc32);

		/**
			\fn QString getImageToSendFileName(uint32_t size, uint32_t crc32)
			Szuka zakolejkowanego obrazka.
			\return nazwê pliku znalezionego obrazka. Zwraca ci±g pusty,
			je¶li obrazek nie zosta³ w tej sesji zakolejkowany do wys³ania.
		**/
		QString getImageToSendFileName(uint32_t size, uint32_t crc32);

		/**
			\fn QString saveImage(UinType sender, uint32_t size, uint32_t crc32, const QString& filename, const char* data)
			Zapisuje obrazek w katalogu .gg/images.
			\return pe³n± ¶cie¿kê do zapisanego obrazka.
		**/
		QString saveImage(UinType sender, uint32_t size, uint32_t crc32, const QString& filename, const char* data);

		/**
			\fn QString getSavedImageFileName(uint32_t size, uint32_t crc32)
			Szuka zapisanego obrazka.
			\return zwraca jego nazwê pliku wraz ze ¶cie¿k±. Zwraca ci±g pusty,
			je¶li obrazek nie zosta³ w tej sesji zapisany.
		**/
		QString getSavedImageFileName(uint32_t size, uint32_t crc32);

		/**
			\fn QString replaceLoadingImages(const QString& text, UinType sender, uint32_t size, uint32_t crc32)
			Podmienia obrazki klepsydr na w³a¶ciwy obrazek, o ile zosta³ on ju¿ w ca³o¶ci pobrany.
			Funkcjê t± nale¿y wywo³aæ dla ka¿dego niepowtarzalnego obrazka w tre¶ci dokumentu.
			\param text tre¶æ dokumentu ze znacznikami HTML oznaczaj±cymi klepsydry
			\param sender UIN kontaktu który przysy³a obrazki
			\param size rozmiar obrazka który chcemy wstawiæ
			\param crc32 suma kontrolna obrazka który chcemy wstawiæ
			\return tre¶æ dokumentu z podmienionymi znacznikami HTML
		**/
		QString replaceLoadingImages(const QString& text, UinType sender, uint32_t size, uint32_t crc32);
};

/**
	\var extern GaduImagesManager gadu_images_manager
	Obiekt zarz±dzaj±cy obrazkami przesy³anymi przez protokó³ Gadu-Gadu.
**/
extern GaduImagesManager gadu_images_manager;

/**
	Klasa s³u¿aca do pogl±du obrazków podczas wyboru pliku obrazka.
	\class PixmapPreview
	\brief Podgl±d obrazków
**/
class PixmapPreview : public QLabel, public QFilePreview
{
	public:
		/**
			\fn PixmapPreview()
			Domy¶lny konstruktor.
		**/
		PixmapPreview();

		/**
			\fn void previewUrl(const QUrl& url)
			Ustawia podgl±d dla danego pliku.
			\param url adres (¶cie¿ka lokalna lub plik w sieci) pliku obrazka
		**/
		void previewUrl(const QUrl& url);
};

/**
	Klasa reprezentuj±ca okno wyboru obrazka do wstawiania.
	\class ImageDialog
	\brief Okno wyboru obrazka
**/
class ImageDialog : public QFileDialog
{
	public:
		/**
			\fn ImageDialog(QWidget* parent)
			Konstruktor tworz±cy okno danym rodzicem.
			\param parent rodzic kontrolki
		**/
		ImageDialog(QWidget* parent);
};

/**
	Zmodyfikowany QTextBrowser specjalnie na potrzeby Kadu.
	Klikniêcie na linku otwiera ustawion± w konfiguracji przegl±darkê.
	W menu kontekstowym jest dodatkowa opcja "Kopiuj lokacjê odno¶nika".
	Dodatkowo poprawka b³êdu w Qt.
	\class KaduTextBrowser
	\brief Wy¶wietlanie wiadomo¶ci
**/
class KaduTextBrowser : public QTextBrowser, QToolTip
{
	Q_OBJECT

	private:
		QString anchor;
		int level;
		/**
			this value stores the current highlighted link
			for use with maybeTip(), or is null
		**/
		QString highlightedlink; 	

	private slots:
		void copyLinkLocation();
		void hyperlinkClicked(const QString& link) const;
		void linkHighlighted(const QString &);
		
	protected:
		/**
			\fn QPopupMenu *createPopupMenu(const QPoint &point)
			Wy¶wietla menu kontekstowe.
			\param point wspó³rzêdne punktu, w którym ma znale¼æ siê lewy górny róg menu kontekstowego
			\return wska¼nik do obiektu menu
		**/
		QPopupMenu *createPopupMenu(const QPoint &point);

		/**
			\fn virtual void drawContents(QPainter * p, int clipx, int clipy, int clipw, int cliph)
			Rysuje zawarto¶æ kontrolki (wiadomo¶ci, separatory, itp).
			\param p wska¼nik do obiektu rysuj±cego
			\param clipx wspó³rzêdne poziome lewego górnego rogu prostok±ta który zostanie narysowany
			\param clipy wspó³rzêdne pionowe lewego górnego rogu prostok±ta który zostanie narysowany
			\param clipw szeroko¶æ prostok±ta który zostanie narysowany
			\param cliph wysoko¶æ prostok±ta który zostanie narysowany
		**/
		virtual void drawContents(QPainter * p, int clipx, int clipy, int clipw, int cliph);

		/**
			\fn virtual void maybeTip(const QPoint& point)
			Funkcja wy¶wietlaj±ca podpowied¼.
			\param point wspó³rzêdne okre¶laj±ce lewy górny róg podpowiedzi
		**/
		virtual void maybeTip(const QPoint&);

		/**
			\fn virtual void contentsMouseReleaseEvent(QMouseEvent * e)
			Emituje sygna³ puszczenia przycisku myszy.
			\param e wska¼nik do obiektu reprezentuj±cego zdarzenie myszy
		**/
		virtual void contentsMouseReleaseEvent(QMouseEvent * e);

	public:
		/**
			\fn KaduTextBrowser(QWidget *parent = 0, const char *name = 0)
			Konstruktor standardowy.
			\param parent rodzic kontrolki (domy¶lnie: 0)
			\param name nazwa kontrolki (domy¶lnie: 0)
		**/
		KaduTextBrowser(QWidget *parent = 0, const char *name = 0);

		/**
			\fn void setSource(const QString &name)
			Ustawia ¼ród³o danych.
			\param name nazwa ¼ród³a danych
			\note Tak na prawdê funkcja ta na obecn± chwilê nie robi nic.
		**/
		void setSource(const QString &name);

		/**
			\fn void clear()
			Czy¶ci zawarto¶æ.
		**/
		void clear() 
		{ 
			setText("");
			//dla QT<3.2 clear() powodowa³o b³±d 
		}

		/**
			\fn void setFont(const QFont& f)
			Ustawia czcionkê.
			\param f obiekt czcionki do ustawienia
		**/
		void setFont(const QFont& f) 	{ QTextBrowser::setFont(f); }

		/**
			\fn void setMargin(int width)
			Ustawia szeroko¶æ marginesu.
			\param width liczba pikseli
		**/
		void setMargin(int width);

	public slots:
		/**
			\fn virtual void copy()
			Kopiuje zaznaczon± czê¶æ do schowka.
		**/
		virtual void copy();

	signals:
		/**
			\fn void mouseReleased(QMouseEvent *e, KaduTextBrowser *sender)
			Dowolny przycisk myszy zosta³ zwolniony.
			\param e wska¼nik do obiektu reprezentuj±cego zdarzenie myszy
			\param sender wska¼nik do obiektu, który wyemitowa³ ten sygna³, czyli \c this
		**/
		void mouseReleased(QMouseEvent *e, KaduTextBrowser *sender);
};

/**
	\fn QValueList<int> toIntList(const QValueList<QVariant> &in)
	Konwertuje ca³± listê wariantów do listy liczb ca³kowitych za jednym zamachem.
**/
QValueList<int> toIntList(const QValueList<QVariant> &in);

/**
	\fn QValueList<int> toIntList(const QValueList<QVariant> &in)
	Konwertuje ca³± listê liczb ca³kowitych do listy wariantów za jednym zamachem.
**/
QValueList<QVariant> toVariantList(const QValueList<int> &in);

/**
	\fn template<class T, class X> QValueList<T> keys(const QMap<T, X> &m)
	Funkcja ta jest reimplementacj± funkcji z Qt - QMap::keys(), w celu
	ominiêcia b³êdu krytycznego w Qt.
**/
template<class T, class X> QValueList<T> keys(const QMap<T, X> &m)
{
#if QT_VERSION < 0x030005
    QValueList<T> ret;
    for(QMap<T,X>::const_iterator it = m.begin(), map_end = m.end(); it != map_end; ++it)
        ret.append(it.key());
    return ret;
#else
    return m.keys();
#endif
}

/**
	\fn template<class T, class X> QValueList<X> values(const QMap<T, X> &m)
	Funkcja ta jest reimplementacj± funkcji z Qt - QMap::values(), w celu
	ominiêcia b³êdu krytycznego w Qt.
**/
template<class T, class X> QValueList<X> values(const QMap<T, X> &m)
{
#if QT_VERSION < 0x030005
    QValueList<X> ret;
    for(QMap<T,X>::const_iterator it = m.begin(), map_end = m.end(); it != map_end; ++it)
        ret.append(it.data());
    return ret;
#else
    return m.values();
#endif
}

/**
	\fn QString narg(const QString &s, const QString &arg1, const QString &arg2, const QString &arg3=QString(), const QString &arg4=QString())
	Zastêpstwo dla arga z QString, które podmienia kolejne %[1-4] w miejscu.

	W QStringu efektem:
	\code QString("%1 odstêp %2").arg("pierwszy %1 tekst").arg("drugi tekst") \endcode
	jest <tt>"pierwszy drugi tekst tekst odstêp %2"</tt>
	a chcieliby¶my ¿eby by³o
	<tt>"pierwszy %1 tekst odstêp drugi tekst"</tt>
	co robi w³a¶nie ta funkcja.
**/
QString narg(const QString &s, const QString &arg1, const QString &arg2,
				const QString &arg3=QString(), const QString &arg4=QString());

/**
	\overload QString narg(const QString &s, const QString &arg1, const QString &arg2, const QString &arg3, const QString &arg4, const QString &arg5, const QString &arg6=QString(), const QString &arg7=QString(),const QString &arg8=QString(), const QString &arg9=QString())
**/
QString narg(const QString &s, const QString &arg1, const QString &arg2,
				const QString &arg3, const QString &arg4,
				const QString &arg5, const QString &arg6=QString(),
				const QString &arg7=QString(),const QString &arg8=QString(),
				const QString &arg9=QString());

/**
	\overload QString narg(const QString &s, const QString **tab, int count)
	Uogólniony narg() na wiêksz± liczbê parametrów.
	\param count musi byæ mniejszy lub równy 9
	\param tab tablica \c count wska¼ników do QString
**/
QString narg(const QString &s, const QString **tab, int count);

/**
	\fn void printBacktrace(const QString &header="")
	Wypisuje na wyj¶ciu standardowym tzw. <i>backtrace</i>.
	\param header opcjonalna nazwa nag³ówka
**/
void printBacktrace(const QString &header="");

/**
	\def VAR(v,x)
	Definiuje now± zmienn± v o takim samym typie jak x jednocze¶nie inicjuj±c j± warto¶ci± x.
**/
#define VAR(v,x)	__typeof(x) v=x

/**
	\def FOREACH(i,c)
	Szybsza w zapisaniu pêtla \c for .
**/
#define FOREACH(i,c) for(VAR(i, (c).begin()); i!=(c).end(); ++i)

/**
	\def CONST_FOREACH(i,c)
	Szybsza w zapisaniu pêtla \c for , operuj±ca na iteratorach sta³ych lub dynamicznych, w zale¿no¶ci od wersji Qt.
**/
#if QT_VERSION >= 0x030200
#define CONST_FOREACH(i,c) for(VAR(i, (c).constBegin()), _kadu__##i##__end = (c).constEnd(); i!=_kadu__##i##__end; ++i)
#else
#define CONST_FOREACH(i,c) for(VAR(i, (c).begin()), _kadu__##i##__end = (c).end(); i!=_kadu__##i##__end; ++i)
#endif

#endif
