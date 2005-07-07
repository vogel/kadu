#ifndef CHAT_H
#define CHAT_H

#include <qcolor.h>
#include <qdialog.h>
#include <qmap.h>
#include <qmultilineedit.h>
#include <qpushbutton.h>
#include <qrect.h>
#include <qsplitter.h>
#include <qvariant.h>
#include <qvaluelist.h>

#include "emoticons.h"
#include "misc.h"
#include "userbox.h"

class Chat;
class QHBox;
class ChatSlots;
/** \typedef QValueList<Chat*> ChatList */
typedef QValueList<Chat*> ChatList;

/**

	Klasa przechowuj±ca informacje o wiadomo¶ci, która ma siê pojawiæ
	w oknie Chat.
	\class ChatMessage
	\brief Klasa przechowuj±ca informacje o wiadomo¶ci.
**/
class ChatMessage
{
	public:
	QString nick;/*!< nazwa u¿ytkownika */
	QDateTime date; /*!< data otrzymania wiadomo¶ci */
	QDateTime sdate; /*!< data wys³ania wiadomo¶ci */
	QString unformattedMessage; /*!< niesformatowana wiadomo¶æ */
	bool isMyMessage; /*!< zmienna mowi±ca czy wiadomo¶c zosta³a 
				napisana przez nas */
	QColor backgroundColor; /*!< t³o wiadomo¶ci */
	QColor textColor; /*!< kolor wiadomo¶ci */
	
	
	QMap<QString, bool> attributes; /*!<
		Mo¿e s³u¿yæ do przechowywania informacji o tym
		czy wiadomo¶æ by³a potwierdzona, szyfrowana, ...
		W zamy¶le ma s³u¿yæ do okre¶lania czy jaki¶ obrazek
		ma byæ dodawany do wiadomo¶ci czy nie - jaki obrazek -
		ta informacja bêdzie gdzie indziej
		*/
	
	//inne atrybuty?
	//QMap<QString, QString> stringAttributes;
	
	bool needsToBeFormatted;/*!< zmienna mowi±ca czy wiadomo¶c powinna 
				  byæ sformatowana */
	
	QString message;/*!<
			Sformatowana wiadomo¶æ (razem z \<p\> lub \<table\>)
			**/


	/**
		\fn ChatMessage(const QString &nick, const QString &unformattedMessage, bool myMessage, QDateTime date, QDateTime sdate=QDateTime())
		Konstruktor ustawiaj±cy nick, niesformatowan± wiadomo¶æ,
		czy wiadomo¶æ pochodzi od nas, datê wiadomo¶ci,
		datê nadania wiadomo¶ci przez serwer
		\param nick nazwa u¿ytkownika
		\param unformattedMessage niesformatowana wiadomo¶æ
		\param myMessage zmienna mowi±ca czy wiadomo¶æ pochodzi od nas
		\param date data otrzymania wiadomo¶ci
		\param sdate data wys³ania wiadomo¶ci
	**/
	ChatMessage(const QString &nick, const QString &unformattedMessage, bool myMessage, QDateTime date, QDateTime sdate=QDateTime());
	/**
		\fn ChatMessage(const QString &formattedMessage, const QColor &bgColor=Qt::white, const QColor &textColor=Qt::black)
		Konstruktor ustawiaj±cy sformatowan± wiadomo¶æ,
		kolor t³a i liter wiadomo¶ci
		\param nick nazwa u¿ytkownika
		\param unformattedMessage niesformatowana wiadomo¶æ
		\param myMessage zmienna mowi±ca czy wiadomo¶æ pochodzi od nas
		\param date data otrzymania wiadomo¶ci
		\param sdate data wys³ania wiadomo¶ci
	**/
	ChatMessage(const QString &formattedMessage, const QColor &bgColor=Qt::white, const QColor &textColor=Qt::black);
};
/**
	Klasa pozwalaj±ca zarz±dzaæ otwartymi oknami rozmów: otwieraæ,
	zamykac, szukaæ okna ze wglêdu na listê u¿ytkowników itp.
	\class ChatManager
	\brief Klasa zarz±dzaj±ca oknami Chat
**/
class ChatManager : public QObject
{
	Q_OBJECT

	private:
		ChatList Chats;/*!< lista okien*/
		
		static ChatSlots *chatslots;
		/**
		\fn int openPendingMsg(int index, ChatMessage &msg)
		Funkcja otwieraj±ca zakolejkowan± wiadomo¶æ
		\param index nr wiadomo¶ci z kolejki
		\param msg wiadomo¶æ z kolejki
		\return zwracany jest nr okna w którym zosta³a otwarta wiadomo¶æ
		**/
		int openPendingMsg(int index, ChatMessage &msg);
		
		/** 
		\struct ChatInfo
		Struktura przechowuje informacje o danym oknie
		**/
		struct ChatInfo
		{
			UinsList uins;/*!< lista u¿ytkowników 
					identyfikuj±ca okno */
			QMap<QString, QVariant> map; /*!< parametry 
							danego okna */
		};
		QValueList<ChatInfo> addons; /*!< lista parametrów okien */

	public:	
		/**
		\fn ChatManager(QObject* parent=NULL, const char* name=NULL)
		Konstruktor tworz±cy obiekt zarz±dzaj±cy oknami
		\param parent rodzic okna
		\param name nazwa obiektu
		**/
		ChatManager(QObject* parent=NULL, const char* name=NULL);
		
		/**
			\fn ~ChatManager()
			Destruktor zamyka wszystkie otwarte okna
		**/
		
		~ChatManager();
		
		/**
			\fn const ChatList& chats() const
			Funkcja zwraca liste otwartych okien Chat
		**/

		/**
			\fn static void initModule()
			Rejestruje opcje modulu Chat w oknie konfiguracji
		**/
		static void initModule();
		static void closeModule();

		const ChatList& chats() const;
		
		/**
			\fn Chat* findChatByUins(const UinsList &uins) const
			Funkcja zwraca wska¼nik do okna z list± 
			u¿ytkowników uins
			\param uins lista u¿ytkowników
			\return wska¼nik do okna je¶li istnieje w przeciwnym
			 wypadku zwraca NULL
		**/
		Chat* findChatByUins(const UinsList &uins) const;
		
		/**
			\fn QVariant& getChatProperty(const UinsList &uins, const QString &name)
			Funkcja zwraca warto¶æ w³asno¶ci "name" okna 
			okre¶lonego przez uins
			\param uins lista u¿ytkowników identyfikuj±ca okno
			\param name nazwa w³asno¶ci
			\return zwraca warto¶æ w³asno¶ci je¶li okre¶lone okno 
			istnieje,\n je¶li nie to tworzy tak± 
			w³asno¶æ (ustawia na pust±)
		**/
		QVariant& getChatProperty(const UinsList &uins, const QString &name);
		
	public slots:
	
		/**
			\fn void chatMsgReceived(UinsList senders,const QString& msg,time_t time,bool& grab)
			Slot informuj±cy o odebraniu wiadomo¶ci
			\param senders lista u¿ytkowników identyfikuj±cych okno
			\param msg tre¶æ otrzymanej wiadomo¶ci
			\param time czas otrzymania wiadomo¶ci
			\param grab 
		**/
		void chatMsgReceived(UinsList senders,const QString& msg,time_t time,bool& grab);

		/**
			\fn int openChat(UinsList senders,time_t time=0)
			Funkcja otwiera nowe okno Chat z wymienionymi rozmowcami.
			\param senders lista u¿ytkowników identyfikuj±cych okno
			\param time parametr sluzy do sprawdzenia ile wiadomosci
			z historii ma sie pojawic w oknie.
			\return zwracany jest numer otwartego okna 
		**/
		int openChat(UinsList senders,time_t time=0);		
		
		/**
			\fn void openPendingMsgs(UinsList uins)
			Funkcja wpisuje zakolejkowane wiadomo¶ci do okna
			z u¿ytkownikami "uins"
			\param uins lista u¿ytkowników identyfikuj±cych okno
		**/
		void openPendingMsgs(UinsList uins);
		
		/**
			\fn void openPendingMsgs()
			Funkcja wpisuje wszystkie zakolejkowane wiadomo¶ci 
			do odpowiednich okien
		**/
		void openPendingMsgs();
		
		//TODO: opisac funkcje sendMessage(..)
		/*
			Niebardzo rozumiem tej funkcji (czemu jest uin i uins)	
		*/
		void sendMessage(UinType uin,UinsList selected_uins);
		
		/**
			\fn void closeAllWindows()
			Funkcja zamyka wszystkie okna chat
		**/
		void closeAllWindows();
		
		/**
			\fn int registerChat(Chat* chat)
			Dodaje okno do menad¿era
			\param chat wska¼nik do okna ktore chcemy dodaæ
			\return zwraca numer naszego okna po zarejestrowaniu
		**/
		int registerChat(Chat* chat);
		
		/**
			\fn void unregisterChat(Chat* chat)
			Funkcja wyrejestrowuje okno z managera \n
			Zapisuje w³asno¶ci okna \n
			wysy³a sygna³ chatDestroying i chatDestroyed
			\param chat okno które bêdzie wyrejestrowane
		**/
		void unregisterChat(Chat* chat);
		
		/**
			\fn void refreshTitles()
			Funkcja od¶wie¿a tytu³y wszystkich okien
		**/
		void refreshTitles();
		
		/**
			\fn void refreshTitlesForUin(UinType uin)
			Funkcja od¶wie¿a tytu³y okien które zawieraj± uin
			\param uin nr u¿ytkownika, którego 
			opis/status bêdzie od¶wie¿any
		**/
		void refreshTitlesForUin(UinType uin);
		
		/**
			\fn void changeAppearance()
			Funkcja od¶wie¿a kolory i czcionki we wszystkich oknach
		**/
		void changeAppearance();
		
		/**
			\fn void setChatProperty(const UinsList &uins, const QString &name, const QVariant &value)
			Funkcja pozwala przypisaæ okre¶lonemu czatowi 
			(nawet je¿eli on jeszcze nie istnieje) pewne w³asno¶ci
			\param uins lista u¿ytkowników identyfikuj±cych okno
			\param name nazwa w³asno¶ci
			\param value warto¶æ w³asno¶ci
		**/
		void setChatProperty(const UinsList &uins, const QString &name, const QVariant &value);
		
	signals:
		
		/**
			\fn void chatCreated(const UinsList& senders)
		 	Sygna³ ten jest wysy³any po utworzeniu nowego okna chat
			\param senders lista uin-ów z którymi tworzymy nowy chat
		**/
		void chatCreated(const UinsList& senders);
		
		/**
			\fn void chatDestroying(const UinsList& senders)
		 	Sygna³ ten jest wysy³any przed zamnkniêciem okna chat
			\param senders lista uin-ów które wystêpuj± w tym chacie
		**/
		void chatDestroying(const UinsList& senders);
		
		/**
			\fn void chatDestroyed(const UinsList& senders)
		 	Sygna³ ten jest wysy³any po zamnkniêciem okna chat
			\param senders lista uin-ów które 
			wystêpowa³y w tym chacie
		**/
		void chatDestroyed(const UinsList& senders);
		
		/**
			\fn void chatOpen(const UinsList& senders)
			Sygna³ ten jest wysy³aniy podczas ka¿dej próby 
			otwarcia nowego okna chat nawet je¶li ju¿ taki istnieje
			\param senders lista uin-ów
		**/
		void chatOpen(const UinsList& senders);
};

/** 
	wska¼nik do obiektu ChatManager'a 
**/
extern ChatManager* chat_manager; 

class EmoticonSelector;
class ColorSelector;
/**
	\class CustomInput
	\brief Klasa umo¿liwiaj±ca wpisywanie wiadomo¶ci
**/
class CustomInput : public QMultiLineEdit
{
	Q_OBJECT

	protected:
		
		bool autosend_enabled;/*!< zmienna informuj±ca czy w³±czone jest 
					automatyczne wysy³anie */
		/**
			\fn virtual void keyPressEvent(QKeyEvent *e)
			Funkcja obs³uguj±ca naci¶niêcie przycisku
			\param e wska¼nik do obiektu obs³uguj±cego klawisze
		**/
		virtual void keyPressEvent(QKeyEvent *e);
		
		/**
			\fn virtual void keyReleaseEvent(QKeyEvent *e)
			Funkcja obs³uguj±ca zwolnienie przycisku
			\param e wska¼nik do obiektu obs³uguj±cego klawisze
		**/
		virtual void keyReleaseEvent(QKeyEvent* e);

	public:
		
		/*! Typ wyliczeniowy mowi±cy o rodzaju pisanych znaków */
		enum
		{
			KEY_BOLD,/*!< pogrubienie */
			KEY_ITALIC,/*!< kursywa */
			KEY_UNDERLINE/*!< podkre¶lenie */
		};
		
		/**
			\fn CustomInput(QWidget* parent = 0, const char* name = 0)
			Konstruktor tworz±cy obiekt 
			i ustawiaj±cy odpowiedni styl
			\param parent rodzic okna
			\param name nazwa obiektu
		**/
		CustomInput(QWidget* parent = 0, const char* name = 0);

	public slots:
		
		/**
			\fn void paste()
			Slot wkleja czysty tekst
		**/
		void paste();
		
		/**
			\fn void setAutosend(bool on)
			Ustawia/wy³±cza automatyczne wysy³anie wiadomo¶ci
			\param on zmienna ustawiaj±ca autosend
		**/
		void setAutosend(bool on);

	signals:
		
		/**
			\fn void sendMessage()
			Sygna³ zostaje wys³any kiedy naci¶niêto 
			klawisz wys³ania wiadomo¶ci
		**/
		void sendMessage();
		
		/**
			\fn void specialKeyPressed(int key)
			Sygna³ zostaje wys³any gdy naci¶niêto specjalny 
			klawisz (skrót) odpowiadaj±cy np KEY_BOLD
			\param key warto¶æ z typu wyliczeniowego KEY_*
		**/
		void specialKeyPressed(int key);
		
		/**
			\fn void keyPressed(QKeyEvent* e, CustomInput* sender)
			Dowolny przycisk klawiatury zosta³ naci¶niêty.
			Przekazany zostaje tak¿e obiekt, który wywo³a³ akcjê -
			 czyli this
			\param e wska¼nik do obiektu obs³uguj±cego klawisze
			\param sender wska¼nik do obiektu, który
			wywo³a³ ten sygna³
		**/
		void keyPressed(QKeyEvent* e, CustomInput* sender);
		
		/**
			\fn void keyReleased(QKeyEvent* e, CustomInput* sender)
			Dowolny przycisk klawiatury zosta³ zwolniony.
			Przekazany zostaje tak¿e obiekt, który wywo³a³ akcjê -
			 czyli this.
			\param e wska¼nik do obiektu obs³uguj±cego klawisze
			\param sender wska¼nik do obiektu, który
			wywo³a³ ten sygna³
		**/
		void keyReleased(QKeyEvent* e, CustomInput* sender);
};

/**
	Klasa wykorzystana do oddzielenia listy u¿ytkowników od historii
	i pola do wpisywania wiadomo¶ci
	\class KaduSplitter
	\brief Klasa rozdzielaj±ca kontrolki

**/
class KaduSplitter : public QSplitter
{
	protected:
		QValueList<KaduTextBrowser*> textbrowsers;
		void drawContents(QPainter* p);
		void childEvent(QChildEvent* c);

	public:
	/**
		Konstruktor tworz±cy obiekt
		\fn KaduSplitter(QWidget* parent = 0, const char* name = 0)
		\param parent rodzic okna
		\param name nazwa obiektu

	**/
		KaduSplitter(QWidget* parent = 0, const char* name = 0);
	
	/**
		Konstruktor tworz±cy obiekt o danej orientacji
		\fn KaduSplitter(Orientation o, QWidget* parent = 0, const char* name = 0)
		\param parent rodzic okna
		\param name nazwa obiektu
		\param o orientacja separatora
	**/
		KaduSplitter(Orientation o, QWidget* parent = 0, const char* name = 0);
};

class QMimeSourceFactory;

/**
	Klasa tworz±ca okno rozmowy, rejestruj±ca przyciski,
	formatuj±ca wiadomo¶æ itp.
	\class Chat
	\brief Okno rozmowy
**/
class Chat : public QWidget
{
	Q_OBJECT

	private:
		friend class ChatManager;
		/**
		\struct RegisteredButton struktura przechowuj±ca informacje o zarejestrowanym przycisku
		**/
		struct RegisteredButton
		{
			QString name; /*!< nazwa przycisku */
			QObject* receiver; /*!< obiekt z którego wykorzystana
					bêdzie funkcja do obs³ugi przycisku */
			QString slot; /*!< funkcja podpiêta pod obs³ugê
						tego przycisku */
		};
		static QValueList<RegisteredButton> RegisteredButtons;/*!<
				zarejestrowane przyciski w oknie rozmowy */
		QMap<QString,QPushButton*> Buttons; /*!< przyciski z okna */
		
		QValueList<ChatMessage *> ChatMessages; /*!< wiadomo¶ci wpisane 
							w oknie rozmowy */
	
		UinsList Uins; /*!< lista u¿ytkowników w danym oknie */
		int index;	/*!< nr okna (z chat menad¿era) */
		QString title_buffer; /*!< tytu³ okna */
		QTimer* title_timer;  /*!< zmienna przechowuj±ca czas od ostatniego 
					od¶wie¿enia tytu³u okna */
		QColor actcolor; /*!< zmienna przechowuj±ca aktualny kolor */
		
		QMimeSourceFactory *bodyformat; /*!< zmienna ustawiaj±cy format */
		EmoticonSelector* emoticon_selector; /*!< okienko do 
							wyboru emotikonek */
		ColorSelector* color_selector; /*!< okienko do wyboru koloru */
		QPushButton* boldbtn; /*!< przycisk pogrubiaj±cy czcionkê */
		QPushButton* italicbtn; /*!< przycisk ustawiaj±cy 
					czcionkê na kursywê */			
		QPushButton* underlinebtn; /*!< przycisk podkre¶laj±cy tekst */
		QPushButton* colorbtn; /*!< przycisk umo¿liwiaj±cy 
					wybór koloru czcionki*/
		QPushButton* iconsel;
		QPushButton* autosend; /*!< przycisk automatycznego wysy³ania */
		QPushButton* lockscroll; /*!< przisk blokowania 
						przesuwania rozmowy */
		QAccel* acc; /*!< zmienna potrzebna do
				 rejestracji skrótów klawiszowych*/
		QPushButton* sendbtn; /*!< przycisk do wysy³ania wiadomo¶ci */
		UserBox* userbox; /*!< lista kontaktów przydatna gdy jeste¶my w
						 konferencji */
		QString myLastMessage;/*!< zmienna przechowuj±ca 
					nasz± ostatni± wiadomo¶æ */
		int myLastFormatsLength; /* ?? */
		void* myLastFormats; /* ?? */
		int seq; /* ?? */
		KaduSplitter *vertSplit, *horizSplit; /*!< obiekty oddzielaj±ce
							 kontrolki od siebie */
		int ParagraphSeparator; /* ?? */
		QDateTime lastMsgTime; /*!< czas ostatniej wiadomo¶ci */

		/**
			\fn void pruneWindow()
			Funkcja czyszcz±ca okno rozmowy
		**/
		void pruneWindow(); 

	private slots:
		
		/**
			\fn void userWhois()
			Slot wy¶wietlaj±cy informacje o zaznaczonym kontakcie
		**/
		void userWhois();
		
		/**
			\fn void emoticonSelectorClicked()
			Slot wy¶wietlaj±cy okno z wyborem emotikonek
		**/
		void emoticonSelectorClicked();
		
		/**
			\fn void changeColor()
			Slot zmieniaj±cy kolor czcionki
		**/
		void changeColor();
		
		/**
			\fn void addMyMessageToHistory()
			Slot dodaje wys³an± wiadomo¶æ do historii rozmowy
		**/
		void addMyMessageToHistory();
		
		/**
			\fn void clearChatWindow()
			Slot czyszcz±cy okno rozmowy
		**/
		void clearChatWindow();
		
		/**
			\fn void pageUp()
			Slot przewijaj±cy historiê rozmowy w górê
		**/
		void pageUp();
		
		/**
			\fn void pageDown()
			Slot przewijaj±cy historiê rozmowy w dó³
		**/
		void pageDown();
		
		/**
			\fn void insertImage()
			Slot wklejaj±cy obrazek do okna rozmowy (wybór obrazka)
		**/
		void insertImage();
		
		/**
			\fn void imageReceivedAndSaved(UinType sender,uint32_t size,uint32_t crc32,const QString& path)
			Slot obs³uguj±cy odebranie i zapis obrazka
			\param sender osoba, która wys³a³a obrazek
			\param size rozmiar obrazka
			\param crc32 suma kontrolna obrazka
			\param path ¶cie¿ka do obrazka
		**/
		void imageReceivedAndSaved(UinType sender,uint32_t size,uint32_t crc32,const QString& path);

		/**
			\fn void connectAcknowledgeSlots();
			Slot pod³±czaj±cy potwierdzenie dostarczenia wiadomo¶ci
		**/
		void connectAcknowledgeSlots();
		
		/**
			\fn void disconnectAcknowledgeSlots();
			Slot od³±czaj±cy potwierdzenie dostarczenia wiadomo¶ci
		**/
		void disconnectAcknowledgeSlots();

	protected:
		/**
			\fn virtual void closeEvent(QCloseEvent* e)
			Funkcja obs³uguj±ca zamkniêcie okna
		**/
		virtual void closeEvent(QCloseEvent* e);
		
		/**
			\fn QString convertCharacters(QString text, const QColor &bgcolor, EmoticonsStyle style)
			Funkcja zamienia specjalne tagi na emotikonki,
			html na czysty tekst itp.
			\param text tekst do konwersji
			\param bgcolor kolor t³a
			\param style styl emotikonek
		**/
		QString convertCharacters(QString text, const QColor &bgcolor, EmoticonsStyle style);
		
		/**
			\fn virtual void windowActivationChange(bool oldActive)
			Funkcja steruj±ca mruganiem napisu okna
		**/
		virtual void windowActivationChange(bool oldActive);
		
		/**
			\fn virtual void keyPressEvent(QKeyEvent* e)
			Funkcja obs³uguj±ca naci¶niêcie przycisku
			\param e wska¼nik do obiektu obs³uguj±cego klawisze
		**/
		virtual void keyPressEvent(QKeyEvent* e);
		
		/**
			\fn virtual void keyReleaseEvent(QKeyEvent* e)
			Funkcja obs³uguj±ca zwolnienie przycisku
			\param e wska¼nik do obiektu obs³uguj±cego klawisze
		**/
		virtual void mouseReleaseEvent(QMouseEvent *e);

	public:
		// FIXME - nie powinno byc publicznych zmiennych
		KaduTextBrowser* body; /*!< historia rozmowy */
		CustomInput* edit; /*!< okno do wpisywania wiadomo¶ci */
		QHBox* buttontray;/* ?? */
		
		/**
			Konstruktor okna rozmowy
			\fn Chat(UinsList uins, QWidget* parent = 0, const char* name = 0)
			\param parent rodzic okna
			\param name nazwa obiektu
		**/
		Chat(UinsList uins, QWidget* parent = 0, const char* name = 0);
		
		/**
			\fn ~Chat()
			Destruktor okna rozmowy
		**/
		~Chat();
		
		/**
			\fn static void registerButton(const QString& name, QObject* receiver, const QString& slot)
			Rejestruje przycisk w oknie chat, ustawia funkcje do obs³ugi 
			sygna³u "clicked"
			\param name nazwa przycisku aby moc siê do niego odwo³aæ
			\param receiver obiekt który zawiera funkcje do obs³ugi zdarzenia
			\param slot funkcja która bêdzie obs³ugiwaæ zdarzenie "clicked"
		**/
		static void registerButton(const QString& name, QObject* receiver, const QString& slot);
		
		/**
			\fn static void unregisterButton(const QString& name)
			Wyrejestrowuje przycisk ze wszystkich okien
			\param name nazwa przycisku do wyrejestrowania
		**/
		static void unregisterButton(const QString& name);
		
		/**
			\fn QPushButton* button(const QString& name) const
			Zwraca wska¼nik do przycisku 
			\param name nazwa przycisku
			\return wska¼nik do przycisku, je¶li przycisk 
			nie istnieje zwraca NULL
		**/
		QPushButton* button(const QString& name) const;
		
		/**
			\fn void formatMessages(QValueList<ChatMessage *> &msgs)
			Formatuje wszystkie wiadomo¶ci na html'a,
			dodaje kolory t³a i czcionek
			\param msgs lista wiadomo¶ci do sformatowania
		**/
		void formatMessages(QValueList<ChatMessage *> &msgs);
		/**
			\fn void formatMessage(ChatMessage &msg,
				QColor myBgColor=QColor(), QColor usrBgColor=QColor(),
				QColor myFontColor=QColor(), QColor usrFontColor=QColor(),
				EmoticonsStyle style=(EmoticonsStyle)config_file.readNumEntry("Chat","EmoticonsStyle"))

			Formatuje wiadomo¶æ na html'a
			\param msg wiadomo¶æ która bêdzie sformatowana
			\param myBgColor kolor który bêdzie ustawiony jako t³o
			w naszych wiadomo¶ciach
			\param usrBgColor kolor, który bêdzie ustawiony jako t³o
			wiadomo¶ci naszego rozmówcy
			\param myFontColor kolor naszej czcionki
			\param usrFontColor kolor czcionki naszego rozmówcy
			\param style styl ikonek 
		**/
		void formatMessage(ChatMessage &msg,
				QColor myBgColor=QColor(), QColor usrBgColor=QColor(),
				QColor myFontColor=QColor(), QColor usrFontColor=QColor(),
				EmoticonsStyle style=(EmoticonsStyle)config_file.readNumEntry("Chat","EmoticonsStyle"));
		
		/**
			\fn void checkPresence(UinsList, const QString&, time_t, QValueList<ChatMessage *> &messages)
			Dodaje wiadomo¶æ do okna
			\param senders lista u¿ytkowników
			\param msg wiadomo¶c
			\param time czas 
			\param messages lista wiadomo¶ci do wpisania do okna
		**/
		void checkPresence(UinsList senders, const QString &msg, time_t time, QValueList<ChatMessage *> &messages);
		
		/**
			\fn void writeMessagesFromHistory(UinsList senders, time_t time)
			Wpisuje do okna historiê rozmowy
			\param senders lista u¿ytkowników identyfikuj±cych okno
			\param time ..
		**/
		void writeMessagesFromHistory(UinsList senders, time_t time);
		
		/**
			\fn const UinsList& uins() const
			Zwraca listê numerów rozmowców.
		**/
		const UinsList& uins() const;
		
		/**
			\fn QValueList<ChatMessage*>& chatMessages()
			Daje dostêp do wiadomo¶ci aktualnie przechowywanych
			w oknie chat. Metody tej mo¿na u¿yæ do zmiany tre¶ci
			lub w³a¶ciwo¶ci której¶ z wiadomo¶ci w odpowiedzi
			na jakie¶ zdarzenie.
		**/
		QValueList<ChatMessage*>& chatMessages();
		
		/**
			\fn void repaintMessages()
			Od¶wie¿a zawarto¶æ okna uwzglêdniaj±c ewentualne
			zmiany dokonane w której¶ wiadomo¶ci z listy
			uzyskanej za pomoc± metody chatMessages(),
			dodanie nowych wiadomo¶ci lub usuniêcie istniej±cych.
		**/
		void repaintMessages();
		
		/**
			\fn const QString& title() const
			Zwraca aktualny tytu³ okna
		**/
		const QString& title() const;

	public slots:
		
		/**
			\fn void changeAppearance()
			Slot zmieniaj±cy kolory i czcionki w oknie
		**/
		void changeAppearance();
		
		/**
			\fn void setTitle()
			Slot ustawiaj±cy tytu³ okna zgodnie z konfiguracj±
		**/
		void setTitle();
		
		/**
			\fn void addEmoticon(QString)
			Slot dodaje ikonke do pola wpisywania wiadomo¶ci
			\param emot ikonka np. <lol>
		**/
		void addEmoticon(QString);
		
		/**
			\fn void scrollMessages(const QValueList<ChatMessage *> &)
			Slot dodaj wiadomo¶ci do okna
			\param messages lista wiadomo¶ci
		**/
		void scrollMessages(const QValueList<ChatMessage *> &);
		
		/**
			\fn void alertNewMessage()
			Slot informuj±cy o nowej wiadomo¶ci 
			mruganie je¶li okno jest nieaktywne
		**/
		void alertNewMessage();
		
		/**
			\fn void HistoryBox()
			Slot wywo³ywany po naci¶niêciu przycisku historii
		**/
		void HistoryBox();
		
		/**
			\fn void sendMessage()
			Slot wywo³ywany po naci¶niêciu przycisku 
			do wysy³ania wiadomo¶ci
		**/
		void sendMessage();
		
		/**
			\fn void cancelMessage()
			Slot wywo³ywany po naci¶nieciu przycisku anulowania
			wysy³aniu wiadomo¶ci
		**/
		void cancelMessage();
		
		/**
			\fn void writeMyMessage()
			Slot wpisuj±cy wiadomo¶æ do okna
			\see sendMessage
		**/
		void writeMyMessage();
		
		/**
			\fn void changeTitle()
			Slot zmieniaj±cy tytu³ 
			\see setTitle
		**/
		void changeTitle();
		
		/**
			\fn void toggledBold(bool on)
			Slot wywo³ywany przy naci¶niêciu przycisku pogrubienia
			\param on ustawia pogrubienie
		**/
		void toggledBold(bool on);
		
		/**
			\fn void toggledItalic(bool on)
			Slot wywo³ywany przy naci¶niêciu przycisku kursywy
			\param on ustawia kursywê
		**/
		void toggledItalic(bool on);
		
		/**
			\fn void toggledUnderline(bool on)
			Slot wywo³ywany przy naci¶niêciu przycisku podkre¶lenia
			\param on ustawia podkre¶lenie
		**/
		void toggledUnderline(bool on);
		
		/**
			\fn void curPosChanged(int para, int pos)
			Slot wywo³ywany kiedy pozycja kursosa zmieni³a siê
			\param  para nie obslugiwane
			\param pos nie obslugiwane
		**/
		void curPosChanged(int para, int pos);
		
		/**
			\fn void specialKeyPressed(int key)
			Slot zostaje wywo³any gdy naci¶niêto specjalny klawisz (skrót)
			odpowiadaj±cy np KEY_BOLD
			\param key warto¶æ naci¶niêtego specjalnego klawisza
		**/
		void specialKeyPressed(int key);
		
		/**
			\fn void colorChanged(const QColor& color)
			Slot zostaje wywo³wany gdy u¿ytkownik zmieni³ kolor czcionki
			któr± bêdzie pisa³
			\param color warto¶æ zmienionego koloru
		**/
		void colorChanged(const QColor& color);
		
		/**
			\fn void colorSelectorAboutToClose()
			Slot zostaje wywo³any przy zamykaniu okna wyboru ikonek
		**/
		void colorSelectorAboutToClose();
		
		/**
			\fn void messageBlockedSlot(int seq, UinType uin)
			Slot zostaje wywo³any gdy nasza wiadomo¶æ zosta³a zablokowana
			przez serwer
			\param seq 
			\param uin
		**/
		void messageBlockedSlot(int seq, UinType uin);
		
		/**
			\fn void messageBoxFullSlot(int seq, UinType uin)
			Slot zostaje wywo³any gdy skrzynka na wiadomo¶ci
			zosta³a zape³niona
			\param seq
			\param uin
		**/
		void messageBoxFullSlot(int seq, UinType uin);
		
		/**
			\fn void messageNotDeliveredSlot(int seq, UinType uin)
			Slot zostaje wywo³any gdy wiadomo¶æ nie zosta³a dostarczona
			\param seq
			\param uin
		**/
		void messageNotDeliveredSlot(int seq, UinType uin);
		
		/**
			\fn void messageAcceptedSlot(int seq, UinType uin)
			Slot zostaje wywo³any gdy wiadomo¶æ zosta³a poprawnie wys³ana
			\param seq
			\param uin
		**/
		void messageAcceptedSlot(int seq, UinType uin);
		
	signals:
		/**
			\fn void messageSendRequested(Chat* chat)
			Sygnal jest emitowany gdy uzytkownik wyda polecenie
			wyslania wiadomosci, np klikajac na guzik "wyslij".
			\param chat wska¼nik do okna które emituje sygna³
		**/
		void messageSendRequested(Chat* chat);
		
		/**
			\fn void messageFiltering(const UinsList& uins, QCString& msg, bool& stop)
			Sygnal daje mozliwosc operowania na wiadomoci
			ktora ma byc wyslana do serwera juz w jej docelowej
			formie po konwersji z unicode i innymi zabiegami.
			Tresc wiadomosci mozna zmienic podmieniajac wskaznik
			msg na nowy bufor i zwalniajac stary (za pomoca free).
			Mozna tez przerwac dalsza jej obrobke ustawiajac
			wskaznik stop na true.
			\param uins lista u¿ytkowników
			\param msg wiadomo¶æ 
			\param stop zakoñczenie dalszej obróbki sygna³u
		**/
		void messageFiltering(const UinsList& uins, QCString& msg, bool& stop);
		
		/**
			\fn void messageSent(Chat* chat)
			Sygnal jest emitowany gdy zakonczy sie proces
			wysylania wiadomosci i zwiazanych z tym czynnosci.
			Oczywiscie nie wiemy czy wiadomosc dotarla.
			\param chat wska¼nik do okna rozmowy,
			 które emitowa³o sygna³
		**/		
		void messageSent(Chat* chat);
};

/**
	\class ColoSelectorButton
	\brief Klasa do wyboru koloru za pomoc± przycisku
**/
class ColorSelectorButton : public QPushButton
{
	Q_OBJECT

	private:
		QColor color;/*!< kolor czcionki */

	private slots:
		/**
			\fn void buttonClicked()
			Slot obs³uguj±cy naci¶niêcie przycisku
		**/
		void buttonClicked();

	public:
		/**
			\fn ColorSelectorButton(QWidget* parent, const QColor& qcolor, int width=1, const char *name=0)
			Konstruktor tworz±cy okno do wyboru koloru
		**/
		ColorSelectorButton(QWidget* parent, const QColor& qcolor, int width=1, const char *name=0);

	signals:
		/**
			\fn void clicked(const QColor& color)
			Sygna³ zostaje wys³any po wybraniu koloru
		**/
		void clicked(const QColor& color);
};

/**
	\clas ColorSelector
	\brief Klasa do wyboru kolorów
**/
class ColorSelector : public QWidget
{
	Q_OBJECT

	private slots:
		/**
			\fn void iconClicked(const QColor& color);
			Slot obs³uguj±cy wybranie koloru
		**/
		void iconClicked(const QColor& color);

	protected:
		/**
			\fn void closeEvent(QCloseEvent*)
			Funkcja obs³uguj±ca zamkniêcie okna wyboru kolorów
		**/
		void closeEvent(QCloseEvent*);

	public:
		/**
			\fn ColorSelector(const QColor &defColor, QWidget* parent = 0, const char* name = 0)
			Konstruktor obiektu do wyboru kolorów
		**/
		ColorSelector(const QColor &defColor, QWidget* parent = 0, const char* name = 0);

	public slots:
		/**
			\fn void alignTo(QWidget* w)
		  	Slot wyrównuje pozycjê do widgeta
			\param w wka¼nik do obiektu ktorego bêdziemy wyrównywac
		**/
		void alignTo(QWidget* w);

	signals:
		/**
			\fn void aboutToClose()
			Sygna³ jest emitowany gdy zamykany jest wybór kolorów
		**/
		void aboutToClose();
		
		/**
			\fn void colorSelect(const QColor&)
			Sygna³ jest emitowany gdy wybrano kolor
			
		**/
		void colorSelect(const QColor&);
};
/**
	\class ChatSlots
	\brief Sloty do obs³ugi okna konfiguracji
**/
class ChatSlots :public QObject
{
	Q_OBJECT

	private:
		/**
			\fn void updatePreview()
			Od¶wie¿enie wygl±du
		**/
		void updatePreview();
	public:
		/**
			Konstruktor obiektu
			\param parent rodzic obiektu
			\param name nazwa obiektu
		**/
		ChatSlots(QObject* parent=0, const char* name=0);

	public slots:
		/**
			\fn void onCreateConfigDialog()
			Slot jest wywo³ywany gdy otwierane jest okno konfiguracji
		**/
		void onCreateConfigDialog();
		
		/**
			\fn void onDestroyConfigDialog()
			Slot jest wywo³ywany gdy zamykane jest okno konfiguracji
		**/
		void onDestroyConfigDialog();
		
		/**
			\fn void chooseEmoticonsStyle(int index)
			Slot jest wywo³ywany gdy wybierany jest styl ikonek
			\param index nr stylu emotikonek
		**/
		void chooseEmoticonsStyle(int index);
		
		/**
			\fn void onPruneChat(bool toggled)
			Slot jest wywo³ywany gdy wybierane jest czy otwierane okno
			ma byæ pusty czy nie
			\param toggled 
		**/
		void onPruneChat(bool toggled);
		
		/**
			\fn void onFoldLink(bool toggled)
			\param toggled
		**/
		void onFoldLink(bool toggled);
		
		/**
			\fn void onBlockClose(bool toggled)
			Slot jest wywo³ywany gdy wybierany jest maksymalny czas
			blokowania zamkniêcia okna po otrzymaniu wiadomo¶ci
			\param toggled
		**/
		void onBlockClose(bool toggled);
		
		/**
			\fn void chooseColor(const char* name, const QColor& color)
			Slot jest wywo³ywany gdy zmieniono kolor,
			aby uaktualniæ podgl±d 
			\param name nazwa koloru
			\param color kolor
		**/
		void chooseColor(const char* name, const QColor& color);
		
		/**
			\fn void chooseFont(const char* name, const QFont& font)
			Slot jest wywo³ywany gdy zmieniono czcionke,
			aby uaktualniæ podgl±d 
			\param name nazwa czcionki
			\param font czcionka
		**/
		void chooseFont(const char* name, const QFont& font);
		
		/**
			\fn void findAndSetWebBrowser(int selectedBrowser)
			Slot jest wywo³ywany gdy wybrano z listy przegl±darkê
			\param selectedBrowser nr przegl±darki
		**/
		void findAndSetWebBrowser(int selectedBrowser);
		
		/**
			\fn void findAndSetBrowserOption(int selectedOption)
			Slot jest wywo³ywany gdy aktywowane pole do wpisania opcji
			\param selectedOption wybrana opcja
		**/
		void findAndSetBrowserOption(int selectedOption);
		
		/**
			\fn static void initBrowserOptions(QComboBox *browserCombo, QComboBox *browserOptionsCombo, QLineEdit *browserPath)
			Slot ustawia przegl±darki wraz z opcjami
			\param browserCombo
			\param browserOptionsCombo
			\param browserPath
		**/
		static void initBrowserOptions(QComboBox *browserCombo, QComboBox *browserOptionsCombo, QLineEdit *browserPath);
		
		/**
			\fn static void findBrowser(int selectedBrowser, QComboBox *browserCombo, QComboBox *browserOptionsCombo, QLineEdit *browserPath)
			Slot jest wywo³ywany gdy wybrano z listy przegl±darkê
			\param selectedBrowser
			\param browserCombo
			\param browserOptionsCombo
			\param browserPath
		**/
		static void findBrowser(int selectedBrowser, QComboBox *browserCombo, QComboBox *browserOptionsCombo, QLineEdit *browserPath);
		
		/**
			\fn static void setBrowserOption(int selectedOption, QLineEdit *browserPathEdit, int chosenBrowser)
			Slot ustawia przegl±darki wraz z opcjami
			\param selectedOption
			\param browsePathEdit
			\param chosenBrowser
		**/
		static void setBrowserOption(int selectedOption, QLineEdit *browserPathEdit, int chosenBrowser);
};

#endif
